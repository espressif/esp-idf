FROM ubuntu:18.04

ARG DEBIAN_FRONTEND=noninteractive

# We need libpython2.7 due to GDB tools
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
    gperf \
    lcov \
    libffi-dev \
    libncurses-dev \
    libpython2.7 \
    libusb-1.0-0-dev \
    make \
    ninja-build \
    python3 \
    python3-pip \
    unzip \
    wget \
    xz-utils \
    zip \
  && apt-get autoremove -y \
  && rm -rf /var/lib/apt/lists/* \
  && update-alternatives --install /usr/bin/python python /usr/bin/python3 10 \
  && python -m pip install --upgrade \
    pip \
    virtualenv \
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
ARG CRYPTOGRAPHY_DONT_BUILD_RUST=1

# Bypass to help pip resolver to install the right tool versions
RUN : \
 && python -m virtualenv /opt/esp/python_env/idf4.3_py3.6_env \
 && /opt/esp/python_env/idf4.3_py3.6_env/bin/python -m pip install \
 Flask==0.12.5 \
 Flask-Compress==1.13 \
 Flask-SocketIO==2.9.6 \
 MarkupSafe==2.0.1 \
 Pygments==2.14.0 \
 Werkzeug==0.16.1 \
 bitstring==3.1.9 \
 brotli==1.0.9 \
 cachecontrol==0.12.14 \
 certifi==2023.5.7 \
 cffi==1.15.1 \
 charset-normalizer==2.0.12 \
 click==8.0.4 \
 colorama==0.4.5 \
 construct==2.10.54 \
 contextlib2==21.6.0 \
 cryptography==3.4.8 \
 ecdsa==0.18.0 \
 future==0.18.3 \
 gdbgui==0.13.2.0 \
 gevent==1.5.0 \
 greenlet==2.0.2 \
 idf-component-manager==1.2.3 \
 idna==3.4 \
 importlib-metadata==4.8.3 \
 importlib-resources==5.4.0 \
 itsdangerous==2.0.1 \
 jinja2==3.0.3 \
 kconfiglib==13.7.1 \
 lockfile==0.12.2 \
 msgpack==1.0.5 \
 packaging==21.3 \
 pycparser==2.21 \
 pyelftools==0.29 \
 pygdbmi==0.9.0.2 \
 pyparsing==2.3.1 \
 pyserial==3.5 \
 python-engineio==3.14.2 \
 python-socketio==4.6.1 \
 pyyaml==6.0 \
 reedsolo==1.5.4 \
 requests==2.27.1 \
 requests-file==1.5.1 \
 requests-toolbelt==1.0.0 \
 schema==0.7.5 \
 six==1.16.0 \
 tqdm==4.64.1 \
 typing-extensions==4.1.1 \
 urllib3==1.26.16 \
 zipp==3.6.0

RUN : \
  && update-ca-certificates --fresh \
  && $IDF_PATH/tools/idf_tools.py --non-interactive install required --targets=${IDF_INSTALL_TARGETS} \
  && $IDF_PATH/tools/idf_tools.py --non-interactive install cmake \
  && $IDF_PATH/tools/idf_tools.py --non-interactive install-python-env \
  && rm -rf $IDF_TOOLS_PATH/dist \
  && :

# Ccache is installed, enable it by default
ENV IDF_CCACHE_ENABLE=1
COPY entrypoint.sh /opt/esp/entrypoint.sh

ENTRYPOINT [ "/opt/esp/entrypoint.sh" ]
CMD [ "/bin/bash" ]
