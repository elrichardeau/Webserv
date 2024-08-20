<?php

$uploadDir = $_SERVER["UPLOAD_DIR"];
$files = scandir($uploadDir);

echo '
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="utf-8">
	<title>Delete</title>
	<link rel="stylesheet" type="text/css" href="../stylesDelete.css" />
</head>
<body>
	<div class="row">
		<h1>Choose files :</h1>
		<a class="indexButton" href="../index.html">Index</a>
	</div>
	<ul>';

foreach ($files as $file) {
    if ($file !== '.' && $file !== '..') {
        $filePath = './' . $file;
        echo '
        <li>
            <a class="file" href="' . htmlspecialchars($filePath) . '">' . htmlspecialchars($file) . '</a>
            <button class="delete-button" data-url="' . htmlspecialchars($filePath) . '">&#10005;</button>
        </li>';
    }
}

echo '
	</ul>
	<script src="../listFiles/delete.js"></script>
</body>
</html>';
?>
