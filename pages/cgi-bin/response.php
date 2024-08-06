<?php

$query = $_SERVER['QUERY_STRING'];
parse_str($query, $params);

$name = isset($params['name']) ? $params['name'] : 'N/A';
$age = isset($params['age']) ? $params['age'] : 'N/A';

$html_content = <<<HTML
<!DOCTYPE html>
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
        <div class="tag">Name : <span class="value">$name</span></div>
        <div class="tag">Age : <span class="value">$age</span></div>
    </div>
    <div class="index">
        <a class="indexButton" href="/index.html">Index</a>
    </div>
</body>
</html>
HTML;

echo $html_content;
?>

