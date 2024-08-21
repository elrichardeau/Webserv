<?php

$uploadDir = $_SERVER["UPLOAD_DIR"];
$fileData = stream_get_contents(STDIN);

if (file_put_contents($uploadDir, $fileData) !== false) {
    echo '
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>Upload Result</title>
        <link rel="stylesheet" type="text/css" href="../stylesResponse.css" />
    </head>
    <body>
        <h1 class="pagesTitle">Upload Result</h1>
        <p>Merci pour l\'upload!</p>
        <div class="index">
            <a class="indexButton" href="../index.html">Index</a>
        </div>
    </body>
    </html>';
} else {
    echo 'Failed to move uploaded file.';
}
?>
