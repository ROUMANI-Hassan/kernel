#!/usr/bin/env bash
set -euo pipefail

shopt -s nullglob
zones=(/sys/class/thermal/thermal_zone*)

if ((${#zones[@]} == 0)); then
    echo "No Linux thermal zones were found."
    echo "A virtual machine may not expose the host temperature sensors."
    exit 1
fi

printf '%-18s %-24s %s\n' "ZONE" "TYPE" "TEMPERATURE"
for path in "${zones[@]}"; do
    type=$(<"$path/type")
    temperature=$(<"$path/temp")
    printf '%-18s %-24s %d.%03d C\n' \
        "${path##*/}" "$type" "$((temperature / 1000))" "$((temperature % 1000))"
done
