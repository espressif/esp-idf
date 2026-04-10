idf_component_register(SRCS "ulp_fsm_riscv_combined_main.c"
                    INCLUDE_DIRS "")

#
# ULP FSM support additions to component CMakeLists.txt.
#

set(ulp_fsm_app_name ulp_fsm_main)
set(ulp_fsm_sources "ulp_fsm/main.S")

ulp_embed_binary(${ulp_fsm_app_name} "${ulp_fsm_sources}" "" TYPE fsm)

#
# ULP RISC-V support additions to component CMakeLists.txt.
#

set(ulp_riscv_app_name ulp_riscv_main)
set(ulp_riscv_sources "ulp_riscv/main.c")

ulp_embed_binary(${ulp_riscv_app_name} "${ulp_riscv_sources}" "" TYPE riscv)
