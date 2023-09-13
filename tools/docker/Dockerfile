FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

RUN : \
  && apt-get update \
  && apt-get install -y \
    apt-utils \
    bison \
    ca-certificates \
    ccache \
    check \
    curl \
    flex \
    git \
    git-lfs \
    gperf \
    lcov \
    libbsd-dev \
    libffi-dev \
    libncurses-dev \
    libusb-1.0-0-dev \
    make \
    ninja-build \
    python3 \
    python3-venv \
    ruby \
    unzip \
    wget \
    xz-utils \
    zip \
  && apt-get autoremove -y \
  && rm -rf /var/lib/apt/lists/* \
  && update-alternatives --install /usr/bin/python python /usr/bin/python3 10 \
  && :

# To build the image for a branch or a tag of IDF, pass --build-arg IDF_CLONE_BRANCH_OR_TAG=name.
# To build the image with a specific commit ID of IDF, pass --build-arg IDF_CHECKOUT_REF=commit-id.
# It is possibe to combine both, e.g.:
#   IDF_CLONE_BRANCH_OR_TAG=release/vX.Y
#   IDF_CHECKOUT_REF=<some commit on release/vX.Y branch>.
# Use IDF_CLONE_SHALLOW=1 to peform shallow clone (i.e. --depth=1 --shallow-submodules)
# Use IDF_INSTALL_TARGETS to install tools only for selected chip targets (CSV)

ARG IDF_CLONE_URL=https://github.com/espressif/esp-idf.git
ARG IDF_CLONE_BRANCH_OR_TAG=master
ARG IDF_CHECKOUT_REF=
ARG IDF_CLONE_SHALLOW=
ARG IDF_INSTALL_TARGETS=all

ENV IDF_PATH=/opt/esp/idf
ENV IDF_TOOLS_PATH=/opt/esp

# install build essential needed for linux target apps, which is a preview target so it is installed with "all" only
RUN if [ "$IDF_INSTALL_TARGETS" = "all" ]; then \
    apt-get update \
    && apt-get install -y build-essential \
    && apt-get autoremove -y \
    && rm -rf /var/lib/apt/lists/* ; \
  fi

RUN echo IDF_CHECKOUT_REF=$IDF_CHECKOUT_REF IDF_CLONE_BRANCH_OR_TAG=$IDF_CLONE_BRANCH_OR_TAG && \
    git clone --recursive \
      ${IDF_CLONE_SHALLOW:+--depth=1 --shallow-submodules} \
      ${IDF_CLONE_BRANCH_OR_TAG:+-b $IDF_CLONE_BRANCH_OR_TAG} \
      $IDF_CLONE_URL $IDF_PATH && \
    if [ -n "$IDF_CHECKOUT_REF" ]; then \
      cd $IDF_PATH && \
      if [ -n "$IDF_CLONE_SHALLOW" ]; then \
        git fetch origin --depth=1 --recurse-submodules ${IDF_CHECKOUT_REF}; \
      fi && \
      git checkout $IDF_CHECKOUT_REF && \
      git submodule update --init --recursive; \
    fi

# Install all the required tools
RUN : \
  && update-ca-certificates --fresh \
  && $IDF_PATH/tools/idf_tools.py --non-interactive install required --targets=${IDF_INSTALL_TARGETS} \
  && $IDF_PATH/tools/idf_tools.py --non-interactive install cmake \
  && $IDF_PATH/tools/idf_tools.py --non-interactive install-python-env \
  && rm -rf $IDF_TOOLS_PATH/dist \
  && :

# The constraint file has been downloaded and the right Python package versions installed. No need to check and
# download this at every invocation of the container.
ENV IDF_PYTHON_CHECK_CONSTRAINTS=no

# Ccache is installed, enable it by default
ENV IDF_CCACHE_ENABLE=1

# Install QEMU runtime dependencies
RUN : \
  && apt-get update && apt-get install -y -q \
    bzip2 \
    libglib2.0-0 \
    libpixman-1-0 \
    libslirp0 \
  && rm -rf /var/lib/apt/lists/* \
  && :

# Install QEMU
ARG QEMU_VER=develop_8.0.0_20230522
ARG QEMU_RISCV32_DIST=esp-qemu-riscv32-softmmu-${QEMU_VER}-x86_64-linux-gnu.tar.bz2
ARG QEMU_RISCV32_SHA256=bc7607720ff3d7e3d39f3e1810b8795f376f4b9cf3783c8f2ed3f7f14ba74717
ARG QEMU_XTENSA_DIST=esp-qemu-xtensa-softmmu-${QEMU_VER}-x86_64-linux-gnu.tar.bz2
ARG QEMU_XTENSA_SHA256=a7e5e779fd593cb15f6d197034dc2fb427ed9165a4743e2febc6f6a47dfcc618

RUN bash -c ': \
  && wget --no-verbose https://github.com/espressif/qemu/releases/download/esp-${QEMU_VER//_/-}/${QEMU_RISCV32_DIST} \
  && echo "${QEMU_RISCV32_SHA256} *${QEMU_RISCV32_DIST}" | sha256sum --check --strict - \
  && tar -xf ${QEMU_RISCV32_DIST} -C /opt \
  && rm ${QEMU_RISCV32_DIST} \
  && wget --no-verbose https://github.com/espressif/qemu/releases/download/esp-${QEMU_VER//_/-}/${QEMU_XTENSA_DIST} \
  && echo "${QEMU_XTENSA_SHA256} *${QEMU_XTENSA_DIST}" | sha256sum --check --strict - \
  && tar -xf ${QEMU_XTENSA_DIST} -C /opt \
  && rm ${QEMU_XTENSA_DIST} \
  '
ENV PATH=/opt/qemu/bin:${PATH}

COPY entrypoint.sh /opt/esp/entrypoint.sh
ENTRYPOINT [ "/opt/esp/entrypoint.sh" ]
CMD [ "/bin/bash" ]
