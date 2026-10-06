#!/usr/bin/env bash
# flash.sh — build/flash/monitor with all tunables at the top.
set -euo pipefail

# =============================================================================
# 可改参数（后续只改这一段）
# =============================================================================
IDF_DIR="${HOME}/esp-idf"          # ESP-IDF 仓库路径
TARGET="esp32s31"                  # 芯片型号
PORT="/dev/ttyUSB0"                # 串口；常见还有 /dev/ttyACM0
FLASH_BAUD="921600"                # 烧录波特率
MONITOR_BAUD="115200"              # 监视器波特率
DO_BUILD=1                         # 1: 烧录前先编译；0: 只用已有 build/
DO_FLASH=1                         # 1: 执行烧录
DO_MONITOR=0                       # 1: 烧录后打开串口监视器；0: 烧完即退出
SET_TARGET=0                       # 1: 每次先 set-target（会写 sdkconfig）；平时保持 0
# =============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="${SCRIPT_DIR}"

if [[ ! -f "${IDF_DIR}/export.sh" ]]; then
    echo "error: export.sh not found: ${IDF_DIR}/export.sh" >&2
    exit 1
fi

if [[ "${DO_FLASH}" -eq 1 || "${DO_MONITOR}" -eq 1 ]]; then
    if [[ ! -e "${PORT}" ]]; then
        echo "error: serial port not found: ${PORT}" >&2
        echo "available serial devices:" >&2
        ls -l /dev/ttyUSB* /dev/ttyACM* 2>/dev/null || true
        exit 1
    fi
fi

if [[ -z "${IDF_PATH:-}" ]]; then
    # shellcheck source=/dev/null
    source "${IDF_DIR}/export.sh"
fi

cd "${PROJECT_DIR}"

CMD=(idf.py -p "${PORT}" -b "${FLASH_BAUD}")

if [[ "${SET_TARGET}" -eq 1 ]]; then
    CMD+=(set-target "${TARGET}")
fi
if [[ "${DO_BUILD}" -eq 1 ]]; then
    CMD+=(build)
fi
if [[ "${DO_FLASH}" -eq 1 ]]; then
    CMD+=(flash)
fi
if [[ "${DO_MONITOR}" -eq 1 ]]; then
    CMD+=(monitor --monitor-baud "${MONITOR_BAUD}")
fi

if [[ ${#CMD[@]} -eq 5 ]]; then
    echo "error: nothing to do, enable DO_BUILD / DO_FLASH / DO_MONITOR" >&2
    exit 1
fi

echo "target=${TARGET}  port=${PORT}  flash_baud=${FLASH_BAUD}  monitor_baud=${MONITOR_BAUD}"
echo "+ ${CMD[*]}"
"${CMD[@]}"
