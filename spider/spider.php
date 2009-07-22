<?php
$a = 5;
$c = 6;
$b= &$a;
$b = 4;
echo $a;
$b= &$c;
$b = 8;
echo $a,$c;
?>