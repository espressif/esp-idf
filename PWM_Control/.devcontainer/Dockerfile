FROM espressif/idf

ARG DEBIAN_FRONTEND=nointeractive
ARG CONTAINER_USER=esp
ARG USER_UID=1050
ARG USER_GID=$USER_UID

RUN apt-get update \
  && apt install -y -q \
  cmake \
  git \
  libglib2.0-0 \
  libnuma1 \
  libpixman-1-0 \
  && rm -rf /var/lib/apt/lists/*

# QEMU
ENV QEMU_REL=esp_develop_8.2.0_20240122
ENV QEMU_SHA256=e7c72ef5705ad1444d391711088c8717fc89f42e9bf6d1487f9c2a326b8cfa83
ENV QEMU_DIST=qemu-xtensa-softmmu-${QEMU_REL}-x86_64-linux-gnu.tar.xz
ENV QEMU_URL=https://github.com/espressif/qemu/releases/download/esp-develop-8.2.0-20240122/${QEMU_DIST}

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN wget --no-verbose ${QEMU_URL} \
  && echo "${QEMU_SHA256} *${QEMU_DIST}" | sha256sum --check --strict - \
  && tar -xf $QEMU_DIST -C /opt \
  && rm ${QEMU_DIST}

ENV PATH=/opt/qemu/bin:${PATH}

RUN groupadd --gid $USER_GID $CONTAINER_USER \
  && adduser --uid $USER_UID --gid $USER_GID --disabled-password --gecos "" ${CONTAINER_USER} \
  && usermod -a -G root $CONTAINER_USER && usermod -a -G dialout $CONTAINER_USER

RUN chmod -R 775 /opt/esp/python_env/

USER ${CONTAINER_USER}
ENV USER=${CONTAINER_USER}
WORKDIR /home/${CONTAINER_USER}

RUN echo "source /opt/esp/idf/export.sh > /dev/null 2>&1" >> ~/.bashrc

ENTRYPOINT [ "/opt/esp/entrypoint.sh" ]

CMD ["/bin/bash", "-c"]