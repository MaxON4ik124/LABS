# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(threads-term) begin
(threads-term) PASS
(threads-term) end
EOF
pass;
