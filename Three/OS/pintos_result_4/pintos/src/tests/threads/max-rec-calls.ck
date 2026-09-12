# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(max-rec-calls) begin
(max-rec-calls) PASS
(max-rec-calls) end
EOF
pass;
