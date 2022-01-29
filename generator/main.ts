import { promises as fs } from "fs";
import * as eta from "eta";

let stationName = "StationIO";

fs.readFile(`./configs/${stationName}.json`)
  .then(async (dataBuffer) => {
    eta.configure({ autoEscape: false });
    const data = JSON.parse(dataBuffer.toString());
    await fs.writeFile(
      `./src/${stationName}.main.cpp`,
      (await eta.renderFile(
        (await fs.realpath("./")) + "/templates/main.cpp.eta",
        data
      )) || ""
    );
  })
  .catch((err) => {
    console.error(err);
  });
