use GD;
use Data::Dumper;
use POSIX;

my $image = GD::Image->new(@ARGV[0]) or die $!;

my $image_array_length = POSIX::ceil(($image->height() * $image->width()) / 8);
my $image_array = "const uint8_t image[] = {\n";
$image_array .= $image->width().", ".$image->height()."\n";

my @bits = ();
my $byte_counter = 0;
for(my $y = 0; $y < $image->height(); $y++){
	for(my $x = 0; $x < $image->width(); $x++){
		my ($r, $g, $b) = $image->rgb($image->getPixel($x, $y));
		if($r > 128 and $g > 128 and $b > 128){
			# White pixel
			push(@bits, 0);
		}else{
			# Black pixel
			push(@bits, 1);
		}
		if($#bits == 7){
			$image_array .= ",".bits_to_byte(@bits);
			$byte_counter++;
			if($byte_counter > 20){
				$image_array .= "\n";
				$byte_counter = 0;
			}
			@bits = ();
		}
	}
}
$image_array .= bits_to_byte(@bits) if($#bits != -1);
$image_array .= "\n};";

print($image_array);
exit();

sub bits_to_byte{
	my @bits = @_;
	my $byte = 0;
	$byte += 1 * @bits[7];
	$byte += 2 * @bits[6];
	$byte += 4 * @bits[5];
	$byte += 8 * @bits[4];
	$byte += 16 * @bits[3];
	$byte += 32 * @bits[2];
	$byte += 64 * @bits[1];
	$byte += 128 * @bits[0];
	return sprintf("%3i", $byte);
}