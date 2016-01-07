\set ECHO none
\i sql/can_cpartn.sql
\set ECHO all

CREATE TABLE can_cpartns(id serial primary key, can_cpartn can_cpartn unique);
INSERT INTO can_cpartns(can_cpartn) VALUES('58876-004');
INSERT INTO can_cpartns(can_cpartn) VALUES('000458876');
INSERT INTO can_cpartns(can_cpartn) VALUES('012345678');
INSERT INTO can_cpartns(can_cpartn) VALUES('091234567');
INSERT INTO can_cpartns(can_cpartn) VALUES('020000004');
INSERT INTO can_cpartns(can_cpartn) VALUES('040000002');
INSERT INTO can_cpartns(can_cpartn) VALUES('030000003');
INSERT INTO can_cpartns(can_cpartn) VALUES('10000-000'); -- test limits
INSERT INTO can_cpartns(can_cpartn) VALUES('000000000'); -- not valid?
INSERT INTO can_cpartns(can_cpartn) VALUES('000010000');
INSERT INTO can_cpartns(can_cpartn) VALUES('99999-999');
INSERT INTO can_cpartns(can_cpartn) VALUES('099999999');

SELECT * FROM can_cpartns ORDER BY can_cpartn;

SELECT MIN(can_cpartn) AS min FROM can_cpartns;
SELECT MAX(can_cpartn) AS max FROM can_cpartns;

-- index scan
TRUNCATE can_cpartns;
INSERT INTO can_cpartns(can_cpartn) SELECT '00004'||id FROM generate_series(5678, 8000) id;
SELECT id,can_cpartn::text FROM can_cpartns WHERE can_cpartn = '000048000';

SET enable_seqscan = false;
SELECT id,can_cpartn::text FROM can_cpartns WHERE can_cpartn = '000046000';
SELECT id,can_cpartn FROM can_cpartns WHERE can_cpartn >= '000047000' LIMIT 5;
SELECT count(id) FROM can_cpartns;
SELECT count(id) FROM can_cpartns WHERE can_cpartn <> ('000046500'::text)::can_cpartn;
RESET enable_seqscan;

-- operators and conversions
SELECT '000010000'::can_cpartn < '000010000'::can_cpartn;
SELECT '000010000'::can_cpartn > '000010000'::can_cpartn;
SELECT '000010000'::can_cpartn < '000010001'::can_cpartn;
SELECT '000010000'::can_cpartn > '000010001'::can_cpartn;
SELECT '000010000'::can_cpartn <= '000010000'::can_cpartn;
SELECT '000010000'::can_cpartn >= '000010000'::can_cpartn;
SELECT '000010000'::can_cpartn <= '000010001'::can_cpartn;
SELECT '000010000'::can_cpartn >= '000010001'::can_cpartn;
SELECT '000010000'::can_cpartn <> '000010000'::can_cpartn;
SELECT '000010000'::can_cpartn <> '000010001'::can_cpartn;
SELECT '000010000'::can_cpartn = '000010000'::can_cpartn;
SELECT '000010000'::can_cpartn = '000010001'::can_cpartn;

-- COPY FROM/TO
TRUNCATE can_cpartns;
COPY can_cpartns(can_cpartn) FROM STDIN;
10000-000
099999999
\.
COPY can_cpartns TO STDOUT;

-- clean up --
DROP TABLE can_cpartns;

-- errors
SELECT ''::can_cpartn;
SELECT '100000000'::can_cpartn;
SELECT '!'::can_cpartn;
