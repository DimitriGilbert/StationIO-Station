function daysSinceDate(date, periodStart) {
  return Math.floor((date - periodStart) / msecPerDay);
}

function lnkFrmTpl(HPoffset, HCoffset, date) {
  return `<form id="lnkF">
    <div class="row">
      <div class="form-group col">
        <label for="lnkFHPO">HP offset</label>
        <input type="number" class="form-control" id="lnkFHPO" name="lnkFHPO" value="${HPoffset}">
      </div>
      <div class="form-group col">
        <label for="lnkFHCO">HC offset</label>
        <input type="number" class="form-control" id="lnkFHCO" name="lnkFHCO" value="${HCoffset}">
      </div>
      <div class="form-group col">
        <label for="lnkFDate">period start</label>
        <input type="date" class="form-control" id="lnkFDate" name="lnkFDate" value="${date}">
      </div>
      <div class="col">
        <button class="btn btn-primary" type="button" id="updLnkBtn">Update</button>
      </div>
    </div>
  </form>`;
}

setTimeout(() => {
  linky_utils.addon = {
    hp_offset: getCookie("lnkhpo", 0),
    hc_offset: getCookie("lnkhco", 0),
    date_offset: getCookie("lnkdo", new Date().getFullYear() + "-01-01"),
  };
  let ltp = dcrel("div");
  ltp.innerHTML = lnkFrmTpl(
    linky_utils.addon.hp_offset,
    linky_utils.addon.hc_offset,
    linky_utils.addon.date_offset
  );
  ltp.classList.add("container");
  dgeli("chCtrlC").parentElement.appendChild(ltp);
  setTimeout(() => {
    dgeli("updLnkBtn").addEventListener("click", function (e) {
      let frmd = new FormData(e.target.form);
      linky_utils.addon.hp_offset = frmd.get("lnkFHPO");
      linky_utils.addon.hc_offset = frmd.get("lnkFHCO");
      linky_utils.addon.date_offset = frmd.get("lnkFDate");
    });
    dqsa(".sensor.linky>.snMss>.snMs.hp>.snMs-unit")[0].addEventListener(
      "updated",
      function (e) {
        let tmv =
          (parseFloat(e.target.previousSibling.innerHTML) -
            linky_utils.addon.hp_offset) /
          daysSinceDate(new Date(), new Date(linky_utils.addon.date_offset));
        tmv = Math.round(tmv * 100) / 100;
        e.target.innerHTML += " (" + tmv + " KWh/day)";
      }
    );
    dqsa(".sensor.linky>.snMss>.snMs.hc>.snMs-unit")[0].addEventListener(
      "updated",
      function (e) {
        let tmv =
          (parseFloat(e.target.previousSibling.innerHTML) -
            linky_utils.addon.hc_offset) /
          daysSinceDate(new Date(), new Date(linky_utils.addon.date_offset));
        tmv = Math.round(tmv * 100) / 100;
        e.target.innerHTML += " (" + tmv + " KWh/day)";
      }
    );
  }, 1000);
}, 2500);
