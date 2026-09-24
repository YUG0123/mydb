#!/usr/bin/bash

BASE="http://127.0.0.1:3000"
API_KEY="yug_mydb_2026"

echo "========================================"
echo " AUTHENTICATION TESTS"
echo "========================================"

echo "== create_database with NO API key =="
RESULT=$(curl -s -w "|%{http_code}" \
  -X POST "$BASE/api/create_database" \
  -H 'Content-Type: application/json' \
  -d '{"owner_key":"no_auth_db"}')

CODE="${RESULT##*|}"
BODY="${RESULT%|*}"

echo "Response: $BODY"
echo "HTTP Code: $CODE"

if [ "$CODE" != "401" ]; then
    echo "FAIL: expected 401, got $CODE"
else
    echo "PASS: request without API key was rejected"
fi


echo
echo "== create_database with WRONG API key =="
RESULT=$(curl -s -w "|%{http_code}" \
  -X POST "$BASE/api/create_database" \
  -H 'Content-Type: application/json' \
  -H 'X-API-Key: wrong_key' \
  -d '{"owner_key":"wrong_auth_db"}')

CODE="${RESULT##*|}"
BODY="${RESULT%|*}"

echo "Response: $BODY"
echo "HTTP Code: $CODE"

if [ "$CODE" != "401" ]; then
    echo "FAIL: expected 401, got $CODE"
else
    echo "PASS: request with wrong API key was rejected"
fi


echo
echo "========================================"
echo " EXISTING API TESTS"
echo "========================================"

echo "== create_database =="
curl -s -X POST "$BASE/api/create_database" \
  -H 'Content-Type: application/json' \
  -H "X-API-Key: $API_KEY" \
  -d '{"owner_key":"rachna"}'
echo


echo "== create_database (duplicate, expect error) =="
curl -s -X POST "$BASE/api/create_database" \
  -H 'Content-Type: application/json' \
  -H "X-API-Key: $API_KEY" \
  -d '{"owner_key":"rachna"}'
echo


echo "== update_record (add mayhul) =="
curl -s -X PATCH "$BASE/api/update_record" \
  -H 'Content-Type: application/json' \
  -H "X-API-Key: $API_KEY" \
  -d '{"owner_key":"rachna","name":"mayhul","age":19,"weight":197,"cgpa":9.7}'
echo


echo "== update_record (add rohan) =="
curl -s -X PATCH "$BASE/api/update_record" \
  -H 'Content-Type: application/json' \
  -H "X-API-Key: $API_KEY" \
  -d '{"owner_key":"rachna","name":"rohan","age":20,"weight":250,"cgpa":9.5}'
echo


echo "== display_record (mayhul) =="
curl -s "$BASE/api/display_record?owner_key=rachna&name=mayhul" \
  -H "X-API-Key: $API_KEY"
echo


echo "== display_record (missing, expect 404) =="
curl -s "$BASE/api/display_record?owner_key=rachna&name=nobody" \
  -H "X-API-Key: $API_KEY"
echo


echo "== delete_record (mayhul) =="
curl -s -X DELETE "$BASE/api/delete_record" \
  -H 'Content-Type: application/json' \
  -H "X-API-Key: $API_KEY" \
  -d '{"owner_key":"rachna","name":"mayhul"}'
echo


echo "== display_record after delete (expect 404) =="
curl -s "$BASE/api/display_record?owner_key=rachna&name=mayhul" \
  -H "X-API-Key: $API_KEY"
echo


echo "== update_record on nonexistent db (expect 404) =="
curl -s -X PATCH "$BASE/api/update_record" \
  -H 'Content-Type: application/json' \
  -H "X-API-Key: $API_KEY" \
  -d '{"owner_key":"nope","name":"x","age":1,"weight":1,"cgpa":1}'
echo


echo
echo "========================================"
echo " DUPLICATE UPDATE REGRESSION TEST"
echo "========================================"

echo "== create database duplicate_test =="
curl -s -X POST "$BASE/api/create_database" \
  -H 'Content-Type: application/json' \
  -H "X-API-Key: $API_KEY" \
  -d '{"owner_key":"duplicate_test"}'
echo


echo "== insert test_user with age 19 =="
curl -s -X PATCH "$BASE/api/update_record" \
  -H 'Content-Type: application/json' \
  -H "X-API-Key: $API_KEY" \
  -d '{"owner_key":"duplicate_test","name":"test_user","age":19,"weight":60,"cgpa":9.0}'
echo


echo "== update test_user age from 19 to 25 =="
curl -s -X PATCH "$BASE/api/update_record" \
  -H 'Content-Type: application/json' \
  -H "X-API-Key: $API_KEY" \
  -d '{"owner_key":"duplicate_test","name":"test_user","age":25,"weight":60,"cgpa":9.0}'
echo


echo "== read test_user and verify age == "

RESULT=$(curl -s -w "|%{http_code}" \
  "$BASE/api/display_record?owner_key=duplicate_test&name=test_user" \
  -H "X-API-Key: $API_KEY")

CODE="${RESULT##*|}"
BODY="${RESULT%|*}"

echo "Response: $BODY"
echo "HTTP Code: $CODE"

if [ "$CODE" != "200" ]; then
    echo "FAIL: expected 200, got $CODE"
elif [[ "$BODY" != *'"age":25'* ]]; then
    echo "FAIL: expected age 25"
else
    echo "PASS: duplicate name correctly updated from age 19 to 25"
fi


echo
echo "========================================"
echo " PERSISTENCE TEST"
echo "========================================"

echo "== create persistence_test database =="
curl -s -X POST "$BASE/api/create_database" \
  -H 'Content-Type: application/json' \
  -H "X-API-Key: $API_KEY" \
  -d '{"owner_key":"persistence_test"}'
echo


echo "== insert persistent_user =="
curl -s -X PATCH "$BASE/api/update_record" \
  -H 'Content-Type: application/json' \
  -H "X-API-Key: $API_KEY" \
  -d '{"owner_key":"persistence_test","name":"persistent_user","age":21,"weight":70,"cgpa":9.5}'
echo


echo
echo "== stopping server =="
pkill -f "./server"
sleep 1


echo "== restarting server =="
(nohup ./server > /tmp/test_server.log 2>&1 &)
sleep 1.5


echo "== reading persistent_user after restart =="
RESULT=$(curl -s -w "|%{http_code}" \
  "$BASE/api/display_record?owner_key=persistence_test&name=persistent_user" \
  -H "X-API-Key: $API_KEY")

CODE="${RESULT##*|}"
BODY="${RESULT%|*}"

echo "Response: $BODY"
echo "HTTP Code: $CODE"

if [ "$CODE" != "200" ]; then
    echo "FAIL: expected 200 after restart, got $CODE"
elif [[ "$BODY" != *'"name":"persistent_user"'* ]]; then
    echo "FAIL: record was not restored after restart"
elif [[ "$BODY" != *'"age":21'* ]]; then
    echo "FAIL: incorrect age after restart"
elif [[ "$BODY" != *'"weight":70'* ]]; then
    echo "FAIL: incorrect weight after restart"
elif [[ "$BODY" != *'"cgpa":9.5'* ]]; then
    echo "FAIL: incorrect CGPA after restart"
else
    echo "PASS: record survived server restart"
fi


