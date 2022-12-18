function setUpdatable() {
  document.querySelectorAll(".station-sensor").forEach((el) => {
    let titel = el.firstElementChild.firstElementChild;
    titel.onclick = (ev) => {
      let pn = ev.target.parentNode.parentNode;
      fetch("/?sensor=" + pn.getAttribute("sensor-index"), {
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
        fetch(
          "/?sensor=" + pn2.getAttribute("sensor-index") + "&mesure=" + mi,
          {
            headers: {
              Accept: "text/html",
            },
            method: "GET",
          }
        ).then((res2) => {
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

function formatSensorMesure(sensorName, mesureName, value) {
  let utils = false;
  eval(
    `if(typeof ${sensorName}_utils !== "undefined"){utils=${sensorName}_utils}`
  );
  if (utils && Object.hasOwnProperty.call(utils, "format_" + mesureName)) {
    value = utils["format_" + mesureName](value);
  }
  if (
    !utils ||
    !Object.hasOwnProperty.call(utils, "inChart") ||
    utils.inChart(mesureName)
  ) {
  }
  return value;
}

function sensorMesureInChart(sensorName, mesureName) {
  let value = true;
  let utils = false;
  eval(
    `if(typeof ${sensorName}_utils !== "undefined"){utils=${sensorName}_utils}`
  );
  if (
    utils &&
    Object.hasOwnProperty.call(utils, "inChart") &&
    !utils.inChart(mesureName)
  ) {
    value = false;
  }
  return value;
}

function initChart() {
  let bdy = document.getElementsByTagName("body")[0];

  let chscript = document.createElement("script");
  chscript.setAttribute("src", "https://cdn.jsdelivr.net/npm/chart.js");
  bdy.appendChild(chscript);

  let chelt = document.createElement("canvas");
  chelt.setAttribute("id", "station-chart");
  chelt = bdy.appendChild(chelt);

  const chartCfg = {
    labels: [],
    type: "line",
    data: {},
  };

  getStationData()
    .then((res) => {
      let d = new Date(Date.now());
      chartCfg.labels.push(
        d.getHours() + ":" + d.getMinutes() + ":" + d.getSeconds()
      );
      res
        .json()
        .then((data) => {
          // building datasets
          let datasets = [];
          for (const sensorName in data.sensors) {
            if (Object.hasOwnProperty.call(data.sensors, sensorName)) {
              const sensorMesures = data.sensors[sensorName];
              for (const mesureName in sensorMesures) {
                if (Object.hasOwnProperty.call(sensorMesures, mesureName)) {
                  if (sensorMesureInChart(sensorName, mesureName)) {
                    datasets.push({
                      label: `${sensorName}::${mesureName}`,
                      data: [formatSensorMesure(sensorName, mesureName, sensorMesures[mesureName])],
                      yAxisID: mesureName,
                    });
                  }
                }
              }
            }
          }
          chartCfg.data.datasets = datasets;
          const chart = new Chart(chelt, chartCfg);
          setTimeout(() => {
            updateChart(chart, 22);
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

function updateChart(chart, timer) {
  getStationData()
    .then((res) => {
      let d = new Date(Date.now());
      chart.data.labels.push(
        d.getHours() + ":" + d.getMinutes() + ":" + d.getSeconds()
      );
      res
        .json()
        .then((data) => {
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
                    chart.data.datasets[iSen].data.push(mval);
                    iSen++;
                  }
                  document.querySelectorAll(
                    `.sensor.${sensorName}>.sensor-mesures>.sensor-mesure.${mesureName}>.sensor-mesure-value`
                  )[0].innerHTML = mval;
                }
              }
            }
          }
          chart.update("active");
          if (timer > 0) {
            setTimeout(() => {
              updateChart(chart, timer);
            }, timer * 1000);
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

setTimeout(() => {
  initChart();
}, 1500);

const mq2_utils = {inChart: name => name==="raw"}

