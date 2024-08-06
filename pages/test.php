<?php
echo "Request method: " . $_SERVER["REQUEST_METHOD"] . "\n";
echo "Content length: " . $_SERVER["CONTENT_LENGTH"] . "\n";
echo "Content type: " . $_SERVER["CONTENT_TYPE"] . "\n";
echo "POST Data:\n";
print_r($_POST);
?>
