var customI2CPinsTpl = `<div class="row" id="i2c-config">
  <h2 class="text-center">I2C</h2>
  <div class="col">
    <label for="i2c-data">I2C SDA pin</label>
    <input type="number" class="form-control" name="i2c-data" id="i2c-data">
  </div>
  <div class="col">
    <label for="i2c-clock">I2C SCL pin</label>
    <input type="number" class="form-control" name="i2c-clock" id="i2c-clock">
  </div>
</div>`;

document
  .getElementById("wifi-config")
  .parentElement.appendChild(htmlToElements(customI2CPinsTpl));
