<?php

$dumpFiles = array();
for ($i = 1; $i < count($_SERVER['argv']); $i++) {
	$dumpFiles[] = $_SERVER['argv'][$i];
}

if (!$dumpFiles)
	exit;

$exclude = array(
	'??0exception_handler_base@detail@luabind@@QAE@ABU012@@Z',
	'??0exception_handler_base@detail@luabind@@QAE@XZ',
	'??1exception_handler_base@detail@luabind@@UAE@XZ',
	'??4class_info@luabind@@QAEAAU01@ABU01@@Z',
	'??4exception_handler_base@detail@luabind@@QAEAAU012@ABU012@@Z'
);

echo "LIBRARY XD_LUA\n";
echo "EXPORTS\n";
foreach ($dumpFiles as $file) {
	$fh = fopen($file, 'r');
	while ($line = fgets($fh)) {
		$line = trim($line);
		/*if (!preg_match('/^[0-9A-F]{3,4}\s+[0-9A-F]+\s+SECT.+?External.+?\|\s+(\S+)/', $line, $m))
			continue;
		$symbol = $m[1];
		if (preg_match('/^__/', $symbol))
			continue;
		if (preg_match('/^\?\?_G/', $symbol))
			continue;
		if ((preg_match('/@@[A-Z]/', $symbol) || preg_match('/[Q-Z]/', $symbol)) && !preg_match('/lua/', $symbol))
			continue;
		if (preg_match('/^\?\?[^A-Za-z0-9]/', $symbol))
			continue;
		if (preg_match('/@/', $symbol))
			echo $symbol."\n";
		else
			echo preg_replace('/^_/', '', $symbol)."\n";*/
		$parts = preg_split('/\s+/', $line);
		if (count($parts) < 6)
			continue;
		if (!is_numeric($parts[0]))
			continue;
		if (!preg_match('/^[A-Z0-9]{8}$/', $parts[2]))
			continue;
		$symbol = $parts[3];
		if (preg_match('/^\?\?_/', $symbol))
			continue;
		if (in_array($symbol, $exclude))
			continue;
		echo $symbol."\n";
	}
	fclose($fh);
}