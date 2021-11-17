import { promises as fs } from "fs";
import * as eta from "eta";

let sensorName = "mq2";

fs.readFile(`./generator/${sensorName}.json`)
  .then(async (dataBuffer) => {
    eta.configure({ autoEscape: false });
    const data = JSON.parse(dataBuffer.toString());
    await fs.writeFile(
      `./src/sensors/${sensorName}.cpp`,
      (await eta.renderFile(
        (await fs.realpath("./")) + "/templates/sensor.cpp.eta",
        data
      )) || ""
    );
    await fs.writeFile(
      `./src/sensors/${sensorName}.h`,
      (await eta.renderFile(
        (await fs.realpath("./")) + "/templates/sensor.h.eta",
        data
      )) || ""
    );
  })
  .catch((err) => {
    console.error(err);
  });
