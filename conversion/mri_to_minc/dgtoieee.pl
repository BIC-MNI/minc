#! /usr/local/bin/perl
# Routine to convert data general float to ieee

sub convert_dg_float_to_ieee {
    local($packed_float, $ival, $exponent, $mantissa, @result, $int_result);
    @result = ();
    foreach $packed_float (@_) {
        $ival = unpack('L',$packed_float);
        if ($ival == 0.0) {
            push(@result, 0.0);
        }
        else {
            $exponent = (((($ival >> 24) & 0x7f) - 64) * 4) + 127;
            $mantissa = $ival & 0x00ffffff;
            while ($mantissa && (!( $mantissa & 0x00800000))) {
                $exponent--;
                $mantissa = $mantissa << 1;
            }
            $exponent--;
            $exponent = $exponent << 23;
            $mantissa &= 0x007fffff;       

            $int_result = ($ival & 0x80000000) | $exponent | $mantissa;
            push(@result, unpack('f',pack('L', $int_result)));
        }
                                      
    }
    return @result;
}

foreach $arg (@ARGV) {
    $arg = pack('f', $arg);
}
print join("\n",&convert_dg_float_to_ieee(@ARGV)),"\n";
