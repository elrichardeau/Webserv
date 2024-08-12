import sys

try:
    while True:
        print("ceci une boucle infinie")
except BrokenPipeError:
    sys.stderr.write("Le pipe est fermé. Arrêt du script.\n")
    sys.exit(1)
except KeyboardInterrupt:
    sys.stderr.write("Interruption clavier. Arrêt du script.\n")
    sys.exit(0)