<?php

// $string = file_get_contents("time.json");
// $json_a = json_decode($string, true);

$data = [];
$t = $_POST['matin'] ? $_POST['matin'] : null;
if ($t) {
	$t = explode(":", $t);
	$d = ["h" => $t[0], "m" => $t[1]];
} else {
	// $d = $json_a['matin'];
	$d = ["h" => null, "m" => null];
}
$data['matin'] = $d;

$t = $_POST['midi'] ? $_POST['midi'] : null;
if ($t) {
	$t = explode(":", $t);
	$d = ["h" => $t[0], "m" => $t[1]];
} else {
	// $d = $json_a['midi'];
	$d = ["h" => null, "m" => null];
}
$data['midi'] = $d;

$t = $_POST["apres_midi"] ? $_POST["apres_midi"] : null;
if ($t) {
	$t = explode(":", $t);
	$d = ["h" => $t[0], "m" => $t[1]];
} else {
	// $d = $json_a["apres-midi"];
	$d = ["h" => null, "m" => null];
}
$data["apres_midi"] = $d;

$t = $_POST['soir'] ? $_POST['soir'] : null;

if ($t) {
	$t = explode(":", $t);
	$d = ["h" => $t[0], "m" => $t[1]];
} else {
	// $d = $json_a['soir'];
	$d = ["h" => null, "m" => null];
}
$data['soir'] = $d;

var_dump(json_encode($data, JSON_PRETTY_PRINT));
file_put_contents('time.json', json_encode($data, JSON_PRETTY_PRINT));

// header('Content-type: application/json');
// echo (1);
