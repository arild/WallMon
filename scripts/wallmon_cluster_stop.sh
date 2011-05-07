#!/bin/bash
cluster-fork --bg "pkill $1 wallmond ; pkill $1 wallmons"
pkill wallmond

