export PGOPTIONS='-c default_table_access_method=parquet'
psql -h 127.0.0.1 -p 5432 -U sdb -d sdb.template1 
