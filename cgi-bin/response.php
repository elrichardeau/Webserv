<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Affichage des Données</title>
</head>
<body>
    <h1>Affichage des Données</h1>

    <?php
    if ($_SERVER["REQUEST_METHOD"] == "GET") {
        // Traitement des requêtes GET
        if (isset($_GET['name']) && isset($_GET['age'])) {
            $name = htmlspecialchars($_GET['name']);
            $age = htmlspecialchars($_GET['age']);
            echo "<p>Bonjour $name, j'ai $age ans (GET).</p>";
        } else {
            echo "<p>Données manquantes dans la requête GET !</p>";
        }
    } elseif ($_SERVER["REQUEST_METHOD"] == "POST") {
        // Traitement des requêtes POST
        if (isset($_POST['name']) && isset($_POST['age'])) {
            $name = htmlspecialchars($_POST['name']);
            $age = htmlspecialchars($_POST['age']);
            echo "<p>Bonjour $name, j'ai $age ans (POST).</p>";
        } else {
            echo "<p>Données manquantes dans la requête POST !</p>";
        }
    } else {
        echo "<p>Requête non supportée !</p>";
    }
    ?>
</body>
</html>


