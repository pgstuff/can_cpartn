/*
 * Author: The maintainer's name
 * Created at: Wed Oct 14 23:12:59 -0400 2015
 *
 */

SET client_min_messages = warning;

-- SQL definitions
CREATE TYPE can_cpartn;

-- basic i/o functions
CREATE OR REPLACE FUNCTION can_cpartn_in(cstring) RETURNS can_cpartn AS '$libdir/can_cpartn'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_cpartn_out(can_cpartn) RETURNS cstring AS '$libdir/can_cpartn'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_cpartn_send(can_cpartn) RETURNS bytea AS '$libdir/can_cpartn'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_cpartn_recv(internal) RETURNS can_cpartn AS '$libdir/can_cpartn'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE can_cpartn (
	input = can_cpartn_in,
	output = can_cpartn_out,
	send = can_cpartn_send,
	receive = can_cpartn_recv,
	internallength = 4,
	passedbyvalue
);

-- functions to support btree opclass
CREATE OR REPLACE FUNCTION can_cpartn_lt(can_cpartn, can_cpartn) RETURNS bool AS '$libdir/can_cpartn'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_cpartn_le(can_cpartn, can_cpartn) RETURNS bool AS '$libdir/can_cpartn'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_cpartn_eq(can_cpartn, can_cpartn) RETURNS bool AS '$libdir/can_cpartn'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_cpartn_ne(can_cpartn, can_cpartn) RETURNS bool AS '$libdir/can_cpartn'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_cpartn_ge(can_cpartn, can_cpartn) RETURNS bool AS '$libdir/can_cpartn'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_cpartn_gt(can_cpartn, can_cpartn) RETURNS bool AS '$libdir/can_cpartn'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_cpartn_cmp(can_cpartn, can_cpartn) RETURNS int4 AS '$libdir/can_cpartn'
LANGUAGE C IMMUTABLE STRICT;

-- to/from text conversion
CREATE OR REPLACE FUNCTION can_cpartn_to_text(can_cpartn) RETURNS text AS '$libdir/can_cpartn'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_cpartn_from_text(text) RETURNS can_cpartn AS '$libdir/can_cpartn'
LANGUAGE C IMMUTABLE STRICT;

-- operators
CREATE OPERATOR < (
	leftarg = can_cpartn, rightarg = can_cpartn, procedure = can_cpartn_lt,
	commutator = >, negator = >=,
	restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR <= (
	leftarg = can_cpartn, rightarg = can_cpartn, procedure = can_cpartn_le,
	commutator = >=, negator = >,
	restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR = (
	leftarg = can_cpartn, rightarg = can_cpartn, procedure = can_cpartn_eq,
	commutator = =, negator = <>,
	restrict = eqsel, join = eqjoinsel,
	merges
);
CREATE OPERATOR <> (
	leftarg = can_cpartn, rightarg = can_cpartn, procedure = can_cpartn_ne,
	commutator = <>, negator = =,
	restrict = neqsel, join = neqjoinsel
);
CREATE OPERATOR > (
	leftarg = can_cpartn, rightarg = can_cpartn, procedure = can_cpartn_gt,
	commutator = <, negator = <=,
	restrict = scalargtsel, join = scalargtjoinsel
);
CREATE OPERATOR >= (
	leftarg = can_cpartn, rightarg = can_cpartn, procedure = can_cpartn_ge,
	commutator = <=, negator = <,
	restrict = scalargtsel, join = scalargtjoinsel
);

-- aggregates
CREATE OR REPLACE FUNCTION can_cpartn_smaller(can_cpartn, can_cpartn)
RETURNS can_cpartn
AS '$libdir/can_cpartn'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION can_cpartn_larger(can_cpartn, can_cpartn)
RETURNS can_cpartn
AS '$libdir/can_cpartn'
    LANGUAGE C IMMUTABLE STRICT;

CREATE AGGREGATE min(can_cpartn)  (
    SFUNC = can_cpartn_smaller,
    STYPE = can_cpartn,
    SORTOP = <
);

CREATE AGGREGATE max(can_cpartn)  (
    SFUNC = can_cpartn_larger,
    STYPE = can_cpartn,
    SORTOP = >
);

-- btree operator class
CREATE OPERATOR CLASS can_cpartn_ops DEFAULT FOR TYPE can_cpartn USING btree AS
	OPERATOR 1 <,
	OPERATOR 2 <=,
	OPERATOR 3 =,
	OPERATOR 4 >=,
	OPERATOR 5 >,
	FUNCTION 1 can_cpartn_cmp(can_cpartn, can_cpartn);
-- cast from/to text
CREATE CAST (can_cpartn AS text) WITH FUNCTION can_cpartn_to_text(can_cpartn) AS ASSIGNMENT;
CREATE CAST (text AS can_cpartn) WITH FUNCTION can_cpartn_from_text(text) AS ASSIGNMENT;
