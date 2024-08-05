<?php

if ($_SERVER["REQUEST_METHOD"] == "GET") {
    $query = $_SERVER["QUERY_STRING"];
    $params = array();
    parse_str($query, $params);
    echo '<!DOCTYPE html>
    <html lang="en">
    <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="stylesheet" type="text/css" href="../stylesResponse.css" />
        <title>Response</title>
    </head>
    <body>
        <h1 class="pagesTitle">Response</h1>
        <div class="response">
            <div class="tag">Name : <span class="value">' .$params['name'] . '</span></div>
            <div class="tag">Age : <span class="value">' .$params['age'] . '</span></div>
        </div>
        <div class="index">
            <a class="indexButton" href="/index.html">Index</a>
        </div>
    </body>
    </html>';
} else if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $name = htmlspecialchars($_POST['name']);
    $age = htmlspecialchars($_POST['age']);

    echo '<!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="stylesheet" type="text/css" href="../stylesResponse.css" />
        <title>Response</title>
    </head>
    <body>
        <h1 class="pagesTitle">Response</h1>
        <div class="response">
            <div class="tag">Name : <span class="value">' . $name . '</span></div>
            <div class="tag">Age : <span class="value">' . $age . '</span></div>
        </div>
        <div class="index">
            <a class="indexButton" href="/index.html">Index</a>
        </div>
    </body>
    </html>';
}
?>