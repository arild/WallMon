#!/bin/bash
CMD="rm -f /tmp/*wallmon*"
cluster-fork --bg ${CMD}
${CMD}
