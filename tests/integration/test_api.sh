#!/usr/bin/bash
# Integration test — run this against a running server (./server, port 3000)
# Usage: bash tests/integration/test_api.sh

BASE="http://127.0.0.1:3000"

echo "== create_database =="
curl -s -X POST $BASE/api/create_database -H 'Content-Type: application/json' -d '{"owner_key":"rachna"}'; echo

echo "== create_database (duplicate, expect error) =="
curl -s -X POST $BASE/api/create_database -H 'Content-Type: application/json' -d '{"owner_key":"rachna"}'; echo

echo "== update_record (add mayhul) =="
curl -s -X PATCH $BASE/api/update_record -H 'Content-Type: application/json' \
  -d '{"owner_key":"rachna","name":"mayhul","age":19,"weight":197,"cgpa":9.7}'; echo

echo "== update_record (add rohan) =="
curl -s -X PATCH $BASE/api/update_record -H 'Content-Type: application/json' \
  -d '{"owner_key":"rachna","name":"rohan","age":20,"weight":250,"cgpa":9.5}'; echo

echo "== display_record (mayhul) =="
curl -s "$BASE/api/display_record?owner_key=rachna&name=mayhul"; echo

echo "== display_record (missing, expect 404) =="
curl -s "$BASE/api/display_record?owner_key=rachna&name=nobody"; echo

echo "== delete_record (mayhul) =="
curl -s -X DELETE $BASE/api/delete_record -H 'Content-Type: application/json' \
  -d '{"owner_key":"rachna","name":"mayhul"}'; echo

echo "== display_record after delete (expect 404) =="
curl -s "$BASE/api/display_record?owner_key=rachna&name=mayhul"; echo

echo "== update_record on nonexistent db (expect 404) =="
curl -s -X PATCH $BASE/api/update_record -H 'Content-Type: application/json' \
  -d '{"owner_key":"nope","name":"x","age":1,"weight":1,"cgpa":1}'; echo
