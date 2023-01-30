// const ShrinkCount = [];
// var chartDataLength = Math.ceil(3600/updateTimer);
// var dataShrinkBy = 3;

var updateTimer = 22;

const chartLabels = [];
const chartDatas = [];
var chtDFrom = 0;
var chtDTo = 0;
var dqsa = (i) => document.querySelectorAll(i);
var dcrel = (i) => document.createElement(i);
var dgeli = (i) => document.getElementById(i);
var ohop = (i,e) => Object.prototype.hasOwnProperty.call(i,e);
function setUpdatable() {
  dqsa(".stSn").forEach((el) => {
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

function mkDLabel() {
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

function exstSenUtils(snNm) {
  let u = false;
  eval(
    "if(typeof " +
      snNm +
      '_utils !== "undefined"){u=' +
      snNm +
      "_utils}"
  );
  return u;
}

function frmtSnMes(snNm, msNm, value) {
  let u = exstSenUtils(snNm);
  if (u && ohop(u, "format_" + msNm)) {
    return u["format_" + msNm](value);
  }
  return value;
}

function mesInCht(snNm, msNm) {
  let u = exstSenUtils(snNm);
  return !(
    u &&
    ohop(u, "inChart") &&
    !u.inChart(msNm)
  );
}

function initChart() {
  let bdy = document.getElementsByTagName("body")[0];

  let chscript = dcrel("script");
  chscript.setAttribute("src", "https://cdn.jsdelivr.net/npm/chart.js");
  bdy.appendChild(chscript);

  let chelt = dcrel("canvas");
  chelt.setAttribute("id", "station-chart");
  chelt = bdy.appendChild(chelt);

  let ctrFrm = dcrel("form");
  ctrFrm.setAttribute("id", "chart-control");
  // string litterals do not compile in c++ -_-
  ctrFrm.innerHTML =
    '<div class="row"><div class="col-6"><div class="row"><div class="form-group col-6"><label for="chtDfrom">From</label><input type="range" class="form-range" name="chtDfrom"  id="chtDfrom" value="0" min="0"></div><div class="form-group col-6"><label for="chtDto">To</label><input type="range" class="form-range" id="chtDto" name="chtDto" value="0" min="0"></div></div></div><div class="col-6"><div class="form-group"><label for="updTimer">Update Every</label><input type="number" class="form-control" id="updTimer" value="' +
    updateTimer +
    '" min="5" max="600"></div></div><div class="col-12"><button type="button" class="btn btn-primary" id="upd-cht-btn">Update</button></div></div>';
  ctrFrm = bdy.appendChild(dcrel("div")).appendChild(ctrFrm);
  ctrFrm.parentElement.classList.add("container");
  dgeli("upd-cht-btn").addEventListener("click", (ev) => {
    updateTimer = parseInt(dgeli("updTimer").value);
    chtDFrom = dgeli("chtDfrom").valueAsNumber;
    chtDTo = dgeli("chtDto").valueAsNumber;
    
    buildChart(Chart.getChart("station-chart"));
  });

  const chtCfg = {
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
      for (const snNm in data.sensors) {
        if (ohop(data.sensors, snNm)) {
          const snMess = data.sensors[snNm];
          for (const msNm in snMess) {
            if (ohop(snMess, msNm)) {
              if (mesInCht(snNm, msNm)) {
                let mesVal = frmtSnMes(
                  snNm,
                  msNm,
                  snMess[msNm]
                );
                datasets.push({
                  label: snNm + "::" + msNm,
                  data: [
                    mesVal,
                  ],
                  yAxisID: msNm,
                  pointRadius: 1,
                  pointHoverRadius: 3,
                });

                chartDatas.push([mesVal]);
                
                if (!axeNames.includes(msNm)) {
                  axeNames.push(msNm);
                  axes[msNm] = {
                    id: msNm,
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
      chtCfg.data.datasets = datasets;
      chtCfg.options = {
        responsive: true,
        scales: axes,
      };
      const chart = new Chart(chelt, chtCfg);
      
      chart.data.labels.push(mkDLabel());
      chartLabels.push(mkDLabel());
      
      chart.update();
      setTimeout(() => {
        updateDatas(chart, updateTimer);
      }, 10000);
    })
    // .catch((err1) => {
    //   console.error(err1);
    // });
  })
  // .catch((err) => {
  //   console.error(err);
  // });
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
      for (const snNm in data.sensors) {
        const snMess = data.sensors[snNm];
        for (const msNm in snMess) {
          if (ohop(snMess, msNm)) {
            const mval = frmtSnMes(
              snNm,
              msNm,
              snMess[msNm]
            );
            if (mesInCht(snNm, msNm)) {
              // chart.data.datasets[iSen].data.push(mval);
              chartDatas[iSen].push(mval);
              iSen++;
            }
            let snval = dqsa(
              ".sensor." +
                snNm +
                ">.snMss>.snMs." +
                msNm +
                ">.snMs-value"
            )[0];
            snval.innerHTML = mval;

            let utils = exstSenUtils(snNm);
            if (
              utils &&
              ohop(utils, "format_unit_" + msNm)
            ) {
              let uelt = dqsa(
                ".sensor." +
                  snNm +
                  ">.snMss>.snMs." +
                  msNm +
                  ">.snMs-unit"
              )[0];
              uelt.innerHTML = utils["format_unit_" + msNm](
                uelt.innerHTML
              );
            }
            snval.dispatchEvent(new Event("updated"));
          }
        }
      }
      chartLabels.push(mkDLabel());
      dgeli("chtDfrom")
        .setAttribute("max", chartLabels.length);
      let to = dgeli("chtDto");
      if (to.value == to.getAttribute("max")) {
        to.value = chartLabels.length;
      }
      to.setAttribute("max", chartLabels.length);
      if (chtDTo === 0) {
        chart = buildChart(chart);
      }
      if (updateTimer > 0) {
        setTimeout(() => {
          updateDatas(chart, updateTimer);
        }, updateTimer * 1000);
      }
    })
    // .catch((err1) => {
    //   console.error(err1);
    // });
  })
  // .catch((err) => {
  //   console.error(err);
  // });
}

function buildChart(chart) {
  let displayTo =
    chtDTo < chtDFrom ? chtDTo : chartLabels.length;
  
  chart.data.labels = chartLabels.slice(chtDFrom, displayTo);
  for (let i = 0; i < chart.data.datasets.length; i++) {
    chart.data.datasets[i].data = chartDatas[i].slice(
      chtDFrom,
      displayTo
    );
  }
  chart.update("active");
  return chart;
}

setTimeout(() => {
  initChart();
}, 500);
