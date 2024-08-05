<?php

if (isset($_FILES['userfile'])) {
    $file = $_FILES['userfile'];
    
    if ($file['error'] === UPLOAD_ERR_OK) {
        $uploadDir = '../pages';
        $uploadFile = $uploadDir . basename($file['name']);
        
        if (move_uploaded_file($file['tmp_name'], $uploadFile)) {
            echo '
            <!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <title>Upload Result</title>
                <link rel="stylesheet" type="text/css" href="stylesResponse.css" />
            </head>
            <body>
                <h1 class="pagesTitle">Upload Result</h1>
                <p>Merci pour l\'upload!</p>
                <div class="index">
                    <a class="indexButton" href="index.html">Index</a>
                </div>
            </body>
            </html>';
        } else {
            echo 'Failed to move uploaded file.';
        }
    } else {
        echo 'File upload error: ' . $file['error'];
    }
} else {
    echo 'No file uploaded.';
}
?>
