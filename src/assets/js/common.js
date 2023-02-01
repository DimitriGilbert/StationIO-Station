// declarations
const msecPerDay = 86400000;
const _Station = {
  conf: {
    updTimer: 22,
  },
};

// helpers
var dqsa = (i) => document.querySelectorAll(i);
var dcrel = (i) => document.createElement(i);
var dgeli = (i) => document.getElementById(i);
var ohop = (i, e) => Object.prototype.hasOwnProperty.call(i, e);
var addScr = (s) => {
  let scr = dcrel("script");
  scr.src = s;
  document.body.appendChild(scr);
};
// add script in i ms
var addScrIn = (i, s) => {
  setTimeout(() => {
    addScr(s);
  }, i);
};

// add scripts in i ms then every m ms
var addScrsIn = (i, s, m) => {
  let ni = i;
  for (const scr in s) {
    if (m) {
      ni += m;
    }
    addScrIn(ni, s[scr]);
  }
};

function setCookie(name, value, days) {
  let expires;
  if (days) {
    let date = new Date();
    date.setTime(date.getTime() + days * msecPerDay);
    expires = "; expires=" + date.toGMTString();
  } else {
    expires = "";
  }
  document.cookie = name + "=" + value + expires + "; path=/";
}
function getCookie(name, defVal) {
  let match = document.cookie.match(new RegExp(name + "=([^;]+)"));
  let val = match ? match[1] : null;
  if (val === null) {
    val = defVal;
    setCookie(name, val, 365);
  }
  return val;
}

function fetchData(extras) {
  return fetch(
    "/?format=application/json" + (extras !== undefined ? "&" + extras : ""),
    {
      method: "GET",
    }
  );
}

function exstSenUtils(snNm) {
  let u = false;
  eval("if(typeof " + snNm + '_utils !== "undefined"){u=' + snNm + "_utils}");
  return u;
}

function useSenUtil(sn, util, value) {
  let u = exstSenUtils(sn);
  if (u && ohop(u, util) && typeof u[util] === "function") {
    return u[util](value);
  }
  return value;
}

function updDatas() {
  return new Promise((resolve, reject) => {
    fetchData().then((r) => {
      if (r.ok) {
        r.json().then((d) => {
          // init _Station
          if (_Station.name === undefined) {
            _Station.name = d.name;
            _Station.sensors = {};
            for (let snn in d.sensors) {
              _Station.sensors[snn] = {};
              for (let msn in d.sensors[snn]) {
                _Station.sensors[snn][msn] = [];
              }
            }
          }
          // update _Station
          for (let snn in d.sensors) {
            for (let msn in d.sensors[snn]) {
              _Station.sensors[snn][msn].push(
                useSenUtil(snn, "format_" + msn, d.sensors[snn][msn])
              );
            }
          }
          document.dispatchEvent(new Event("data-updated"));
          if (_Station.conf.updTimer > 0) {
            setTimeout(updDatas, _Station.conf.updTimer * 1000);
          }
          resolve(true);
        });
      }
    });
  });
}

function senMesElt(sn, msn, tgt) {
  let q = ".sensor." + sn + ">.snMss>.snMs." + msn;
  if (tgt) {
    q += ">.snMs-" + tgt;
  }
  let r = dqsa(q);

  if (r.length > 0) {
    return r[0];
  }
  return null;
}

// data display
function updDataDisplay() {
  for (let sn in _Station.sensors) {
    for (let msn in _Station.sensors[sn]) {
      let v = _Station.sensors[sn][msn][_Station.sensors[sn][msn].length - 1];
      let e = senMesElt(sn, msn, "value");
      if (e) {
        e.innerHTML = v;
        e.dispatchEvent(new Event("updated"));
      }
      e = senMesElt(sn, msn, "unit");
      if (e) {
        e.innerHTML = useSenUtil(sn, "format_unit_" + msn, e.innerHTML);
        e.dispatchEvent(new Event("updated"));
      }
    }
  }
}

// chart
var chtFrmTpl = (uTimer) => {
  let ctrFrm = dcrel("form");
  ctrFrm.setAttribute("id", "chart-control");
  // string litterals do not compile in c++ -_-
  ctrFrm.innerHTML =
    '<div class="row"><div class="col-6"><div class="row"><div class="form-group col-6"><label for="chtDfrom">From</label><input type="range" class="form-range" name="chtDfrom"  id="chtDfrom" value="0" min="0"></div><div class="form-group col-6"><label for="chtDto">To</label><input type="range" class="form-range" id="chtDto" name="chtDto" value="0" min="0"></div></div></div><div class="col-6"><div class="form-group"><label for="updTimer">Update Every</label><input type="number" class="form-control" id="updTimer" value="' +
    uTimer +
    '" min="5" max="600"></div></div><div class="col-12"><button type="button" class="btn btn-primary" id="upd-cht-btn">Update</button></div></div>';
  ctrFrm.lastElementChild.lastElementChild.addEventListener("click", (e) => {
    if (_Station.conf.chart === undefined) {
      _Station.conf.chart = {};
    }
    let fd = new FormData(e.target.form);
    _Station.conf.updTimer = parseFloat(fd.get("updTimer"));
    _Station.conf.chart.displayFrom = parseInt(fd.get("chtDfrom"));
    _Station.conf.chart.displayTo = parseInt(fd.get("chtDto"));
  });
  return ctrFrm;
};
var chtMkLabel = (d) => {
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
};

function chtInit() {
  addScr("https://cdn.jsdelivr.net/npm/chart.js");
  let chE = dcrel("canvas");
  chE.setAttribute("id", "station-chart");
  let frmC = dcrel("div").appendChild(chtFrmTpl(_Station.conf.updTimer));
  frmC.setAttribute("id", "chCtrlC");
  frmC.classList.add("container");

  _Station.conf.chart = {
    elt: document.body.appendChild(chE),
    form: document.body.appendChild(frmC),
    labels: [],
    datasets: [],
    datas: [],
    axes: {},
    displayFrom: 0,
    displayTo: 0,
  };

  let axisSide = "left";
  for (const snn in _Station.sensors) {
    const sn = _Station.sensors[snn];
    for (const msn in sn) {
      if (useSenUtil(snn, "inChart", msn)) {
        _Station.conf.chart.datasets.push({
          label: useSenUtil(snn, "label_" + msn, snn + "::" + msn),
          data: [],
          yAxisID: useSenUtil(snn, "axe_" + msn, msn),
          pointRadius: 1,
          pointHoverRadius: 3,
        });
        _Station.conf.chart.datas.push([]);
        if (
          !Object.keys(_Station.conf.chart.axes).includes(
            useSenUtil(snn, "axe_" + msn, msn)
          )
        ) {
          _Station.conf.chart.axes[useSenUtil(snn, "axe_" + msn, msn)] = {
            position: axisSide,
            type: "linear",
            id: useSenUtil(snn, "axe_" + msn, msn),
          };
          axisSide = axisSide === "left" ? "right" : "left";
        }
      }
    }
  }
  // what could possibly go wrong ?
  setTimeout(() => {
    _Station.chart = new Chart(_Station.conf.chart.elt, {
      type: "line",
      labels: _Station.conf.chart.labels,
      data: {
        datasets: _Station.conf.chart.datasets,
      },
      options: {
        responsive: true,
        sclales: _Station.conf.chart.axes,
      },
    });
  }, 250);
}

function chtOnDataUpd() {
  _Station.conf.chart.labels.push(chtMkLabel(new Date()));
  let di = 0;
  let lln = _Station.conf.chart.labels.length - 1;
  let dfrom = _Station.conf.chart.displayFrom;
  let dto = _Station.conf.chart.displayTo;
  if (dto <= dfrom) {
    dto = lln;
  }

  for (const snn in _Station.sensors) {
    for (const msn in _Station.sensors[snn]) {
      if (useSenUtil(snn, "inChart", msn)) {
        let l = _Station.sensors[snn][msn].length - 1;
        let v = _Station.sensors[snn][msn][l];
        console.log(l, v, dfrom, dto);
        _Station.conf.chart.datas[di].push(v);
        _Station.chart.data.datasets[di].data = _Station.conf.chart.datas[
          di
        ].slice(dfrom, dto);
        di++;
      }
    }
  }
  _Station.chart.data.labels = _Station.conf.chart.labels.slice(dfrom, dto);
  dgeli("chtDfrom").setAttribute("max", lln);
  dgeli("chtDto").setAttribute("max", lln);
  _Station.chart.update();
}

// misc
setTimeout(() => {
  updDatas().then(() => {
    console.log("Station Loaded");
    chtInit();
    document.addEventListener("data-updated", chtOnDataUpd);
    document.addEventListener("data-updated", updDataDisplay);
  });
}, 15);
