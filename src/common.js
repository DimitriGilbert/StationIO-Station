// const ShrinkCount = [];
// var chartDataLength = Math.ceil(3600/updateTimer);
// var dataShrinkBy = 3;

var updateTimer = 22;

const chartLabels = [];
const chartDatas = [];
var chartDisplayFrom = 0;
var chartDisplayTo = 0;

function setUpdatable() {
  document.querySelectorAll(".stSn").forEach((el) => {
    let titel = el.firstElementChild.firstElementChild;
    titel.onclick = (ev) => {
      let pn = ev.target.parentNode.parentNode;
      fetch("/?sensor=" + pn.getAttribute("sn-i"), {
        headers: {
          Accept: "text/html",
        },
        method: "GET",
      }).then((res) => {
        res.text().then((nv) => {
          pn.innerHTML = nv;
          setUpdatable();
        });
      });
    };
    let mesures = el.firstElementChild.lastElementChild;
    mesures.childNodes.forEach((mesurel, mi) => {
      mesurel.onclick = (ev) => {
        let pn2 = ev.target.parentNode.parentNode.parentNode.parentNode;
        fetch("/?sensor=" + pn2.getAttribute("sn-i") + "&mesure=" + mi, {
          headers: {
            Accept: "text/html",
          },
          method: "GET",
        }).then((res2) => {
          res2.text().then((nv) => {
            ev.target.parentNode.innerHTML = nv;
            setUpdatable();
          });
        });
      };
    });
  });
}
setTimeout(setUpdatable, 250);

function mkDateLabel() {
  let d = new Date(Date.now());
  return (
    (d.getHours() < 10 ? "0" : "") +
    d.getHours() +
    ":" +
    (d.getMinutes() < 10 ? "0" : "") +
    d.getMinutes() +
    ":" +
    (d.getSeconds() < 10 ? "0" : "") +
    d.getSeconds()
  );
}
/**
 *
 * @returns Promise<Response>
 */
function getStationData() {
  return fetch("/?format=application/json", {
    headers: {
      Accept: "application/json",
    },
    method: "GET",
  });
}

function existsSensorUtils(sensorName) {
  let utils = false;
  eval(
    "if(typeof " +
      sensorName +
      '_utils !== "undefined"){utils=' +
      sensorName +
      "_utils}"
  );
  return utils;
}

function formatSensorMesure(sensorName, mesureName, value) {
  let utils = existsSensorUtils(sensorName);
  if (utils && Object.hasOwnProperty.call(utils, "format_" + mesureName)) {
    return utils["format_" + mesureName](value);
  }
  return value;
}

function sensorMesureInChart(sensorName, mesureName) {
  let utils = existsSensorUtils(sensorName);
  return !(
    utils &&
    Object.hasOwnProperty.call(utils, "inChart") &&
    !utils.inChart(mesureName)
  );
}

function initChart() {
  let bdy = document.getElementsByTagName("body")[0];

  let chscript = document.createElement("script");
  chscript.setAttribute("src", "https://cdn.jsdelivr.net/npm/chart.js");
  bdy.appendChild(chscript);

  let chelt = document.createElement("canvas");
  chelt.setAttribute("id", "station-chart");
  chelt = bdy.appendChild(chelt);

  let ctrFrm = document.createElement("form");
  ctrFrm.setAttribute("id", "chart-control");
  // string litterals do not compile in c++ -_-
  ctrFrm.innerHTML =
    '<div class="row"><div class="col-6"><div class="row"><div class="form-group col-6"><label for="chart-display-from">From</label><input type="range" class="form-range" id="chart-display-from" value="0" min="0"></div><div class="form-group col-6"><label for="chart-display-to">To (same as from for last) </label><input type="range" class="form-range" id="chart-display-to" value="0" min="0"></div></div></div><div class="col-6"><div class="form-group"><label for="data-update-timer">Update Every</label><input type="number" class="form-control" id="data-update-timer" value="' +
    updateTimer +
    '" min="5" max="600"></div></div><div class="col-12"><button type="button" class="btn btn-primary" id="upd-cht-btn">Update</button></div></div>';
  ctrFrm = bdy.appendChild(document.createElement("div")).appendChild(ctrFrm);
  ctrFrm.parentElement.classList.add("container");
  document.getElementById("chart-display-from").addEventListener("change", (ev) => {
    let to = document
      .getElementById("chart-display-to");
    to.setAttribute("min", ev.target.value);
    if (to.value < ev.target.value) {
      to.value = ev.target.value;
    }
  });
  document.getElementById("upd-cht-btn").addEventListener("click", (ev) => {
    updateTimer = parseInt(document.getElementById("data-update-timer").value);
    chartDisplayFrom = document.getElementById("chart-display-from").valueAsNumber;
    chartDisplayTo = document.getElementById("chart-display-to").valueAsNumber;
    
    buildChart(Chart.getChart("station-chart"));
  });

  const chartCfg = {
    labels: [],
    type: "line",
    data: {},
  };

  getStationData().then((res) => {
    res.json().then((data) => {
      // building datasets
      let datasets = [];
      let axeNames = [];
      let axes = {};
      let axisSide = "left";
      for (const sensorName in data.sensors) {
        if (Object.hasOwnProperty.call(data.sensors, sensorName)) {
          const sensorMesures = data.sensors[sensorName];
          for (const mesureName in sensorMesures) {
            if (Object.hasOwnProperty.call(sensorMesures, mesureName)) {
              if (sensorMesureInChart(sensorName, mesureName)) {
                let mesVal = formatSensorMesure(
                  sensorName,
                  mesureName,
                  sensorMesures[mesureName]
                );
                datasets.push({
                  label: sensorName + "::" + mesureName,
                  data: [
                    mesVal,
                  ],
                  yAxisID: mesureName,
                  pointRadius: 1,
                  pointHoverRadius: 3,
                });

                chartDatas.push([mesVal]);
                
                if (!axeNames.includes(mesureName)) {
                  axeNames.push(mesureName);
                  axes[mesureName] = {
                    id: mesureName,
                    type: "linear",
                    position: axisSide,
                  };
                  axisSide = axisSide === "left" ? "right" : "left";
                }
              }
            }
          }
        }
      }
      chartCfg.data.datasets = datasets;
      chartCfg.options = {
        responsive: true,
        scales: axes,
      };
      const chart = new Chart(chelt, chartCfg);
      
      chart.data.labels.push(mkDateLabel());
      chartLabels.push(mkDateLabel());
      
      chart.update();
      setTimeout(() => {
        updateDatas(chart, updateTimer);
      }, 10000);
    })
    .catch((err1) => {
      console.error(err1);
    });
  })
  .catch((err) => {
    console.error(err);
  });
}

// function shrinkChartData(chart, multiplier = 1) {
//   let count =
//     ShrinkCount[multiplier] !== undefined ? ShrinkCount[multiplier] : 0;
//   let max = chartDataLength * multiplier + count;
//   let shrinkBy = 3;
//   if (chart.data.datasets[0].data.length > max) {

//     console.log("current label length", chart.data.labels.length);

//     let labels = chart.data.labels.slice(0, count);
//     labels = labels.concat(chart.data.labels.slice(count + shrinkBy + 1));

//     console.log(labels.length);

//     let datasets = [];

//     console.log("current data length", chart.data.datasets[0].data.length);

//     for (const i in chart.data.datasets) {
//       let dataset = chart.data.datasets[i];
//       let newData = dataset.data.slice(0, count - 1);

//       let old = dataset.data.slice(count, count + shrinkBy);
//       let shrunkVal = 0;
//       for (let i = 0; i < old.length; i++) {
//         shrunkVal += old[i];
//       }
//       shrunkVal /= old.length;

//       newData.push(shrunkVal);
//       newData = newData.concat(dataset.data.slice(count + shrinkBy + 1));

//       console.log("new data length from", newData.length, dataset.data.length);
//       dataset.data = newData;
//       datasets.push(dataset);
//     }

//     console.log("new label length", labels.length);
//     console.log("new data length", datasets[0].length);

//     chart.data.labels = labels;
//     chart.data.datasets = datasets;
//     ShrinkCount[multiplier] = count + 1;
//   }
//   return chart;
// }

function updateDatas(chart) {
  getStationData().then((res) => {
    res.json().then((data) => {
      let iSen = 0;
      for (const sensorName in data.sensors) {
        if (Object.hasOwnProperty.call(data.sensors, sensorName)) {
          const sensorMesures = data.sensors[sensorName];
          for (const mesureName in sensorMesures) {
            if (Object.hasOwnProperty.call(sensorMesures, mesureName)) {
              const mval = formatSensorMesure(
                sensorName,
                mesureName,
                sensorMesures[mesureName]
              );
              if (sensorMesureInChart(sensorName, mesureName)) {
                // chart.data.datasets[iSen].data.push(mval);
                chartDatas[iSen].push(mval);
                iSen++;
              }
              document.querySelectorAll(
                ".sensor." +
                  sensorName +
                  ">.snMss>.snMs." +
                  mesureName +
                  ">.snMs-value"
              )[0].innerHTML = mval;

              let utils = existsSensorUtils(sensorName);
              if (
                utils &&
                Object.hasOwnProperty.call(utils, "format_unit_" + mesureName)
              ) {
                let uelt = document.querySelectorAll(
                  ".sensor." +
                    sensorName +
                    ">.snMss>.snMs." +
                    mesureName +
                    ">.snMs-unit"
                )[0];
                uelt.innerHTML = utils["format_unit_" + mesureName](
                  uelt.innerHTML
                );
              }
            }
          }
        }
      }
      chartLabels.push(mkDateLabel());
      document
        .getElementById("chart-display-from")
        .setAttribute("max", chartLabels.length);
      let to =document
        .getElementById("chart-display-to");
      if (to.value == to.getAttribute("max")) {
        to.value = chartLabels.length;
      }
      to.setAttribute("max", chartLabels.length);
      if (chartDisplayTo === 0) {
        chart = buildChart(chart);
      }
      if (updateTimer > 0) {
        setTimeout(() => {
          updateDatas(chart, updateTimer);
        }, updateTimer * 1000);
      }
    })
    .catch((err1) => {
      console.error(err1);
    });
  })
  .catch((err) => {
    console.error(err);
  });
}

function buildChart(chart) {
  let displayTo =
    chartDisplayTo < chartDisplayFrom ? chartDisplayTo : chartLabels.length;
  
  chart.data.labels = chartLabels.slice(chartDisplayFrom, displayTo);
  for (let i = 0; i < chart.data.datasets.length; i++) {
    chart.data.datasets[i].data = chartDatas[i].slice(
      chartDisplayFrom,
      displayTo
    );
  }
  chart.update("active");
  return chart;
}

setTimeout(() => {
  initChart();
}, 500);
