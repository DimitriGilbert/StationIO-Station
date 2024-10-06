FROM python:3.9-slim

ENV StationIO_docker_PIO_version=6.1.11

LABEL app.name="platformio-core" \
      app.version="${StationIO_docker_PIO_version}"

RUN pip install -U platformio==${StationIO_docker_PIO_version} && \
    mkdir -p /w && \
    mkdir -p /.platformio && \
    chmod a+rwx /.platformio && \
    apt update && apt install -y git && apt-get clean autoclean && apt-get autoremove --yes && rm -rf /var/lib/{apt,dpkg,cache,log}/

WORKDIR /w

ENTRYPOINT ["platformio"] 