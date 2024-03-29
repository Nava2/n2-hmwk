# 
# Synthesis run script generated by Vivado
# 

set_msg_config -id {HDL 9-1061} -limit 100000
set_msg_config -id {HDL 9-1654} -limit 100000
create_project -in_memory -part xc7k70tfbg484-2

set_param project.compositeFile.enableAutoGeneration 0
set_param synth.vivado.isSynthRun true
set_property webtalk.parent_dir /home/kevin/hmwk/ece3390_asn1/ece3390_asn1.cache/wt [current_project]
set_property parent.project_path /home/kevin/hmwk/ece3390_asn1/ece3390_asn1.xpr [current_project]
set_property default_lib work [current_project]
set_property target_language VHDL [current_project]
set_property vhdl_version vhdl_2k [current_fileset]
read_vhdl -library work /home/kevin/hmwk/ece3390_asn1/ece3390_asn1.srcs/sources_1/new/asn1.vhdl
synth_design -top prob1 -part xc7k70tfbg484-2
write_checkpoint -noxdef prob1.dcp
catch { report_utilization -file prob1_utilization_synth.rpt -pb prob1_utilization_synth.pb }
