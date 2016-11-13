#!/bin/bash
# modify_sources_permission.sh

find ./ -name "*.h" | xargs chmod 644
find ./ -name "*.c" | xargs chmod 644
find ./ -name "*.s" | xargs chmod 644
find ./ -name "*.md" | xargs chmod 644
