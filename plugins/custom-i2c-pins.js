var customI2CPinsTpl = `<div class="row" id="i2c-config">
  <div class="col">
    <label for="i2c-data">I2C SDA pin</label>
    <input type="number" class="form-control" name="i2c-data" id="i2c-data">
  </div>
  <div class="col">
    <label for="i2c-clock">I2C SCL pin</label>
    <input type="number" class="form-control" name="i2c-clock" id="i2c-clock">
  </div>
</div>`;

let wifcfgElt = document.getElementById("wifi-config");
wifcfgElt.parentElement.insertBefore(htmlToElements(customI2CPinsTpl), wifcfgElt);
