#set args c13 2000 1 bwct_fw_update /home/ticso/svn/projects/modbus/door_cardreader/firmware/main.bin
#set args c13 2017 255 identification 200
#set args c13 2017 1 write_coils 0 1:1
#set args 10.1.1.245 502 255 write_registers 1 10:10
set args c13 2017 80 write_coils 1 1:0
break main
