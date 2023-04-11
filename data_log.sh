#!/bin/sh 
LOG_FILE="data_log_$(date +%s).log"
echo "Saving data to ${LOG_FILE}"
get_data() {
    (echo "{"; echo "  \"date:\" $(date +%s),"; curl 192.168.2.55 2>/dev/null | tail -n +2; echo "";)
}
while true
do
    get_data | tee -a ${LOG_FILE}
    sleep 2
done
