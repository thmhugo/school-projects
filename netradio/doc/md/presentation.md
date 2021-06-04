# Présentation

## Utilisation normale

1. Lancer un `manager` : `./build/manager 6900 10`.
2. Lancer un `client` : `./run-client.sh client1`.
3. Essayer de demander la liste des `multicaster` enregistrés : `list localhost 6900`.
4. Enregistrer un `multicaster` :
`./build/multicaster CAST1 7000 225.1.2.4:9798 192.168.70.236:6900 config/multcast-mess1.conf`.
5. Redemander la liste des `multicaster` enregistrés : `list localhost 6900`.
6. Écouter les messages diffusés avec le `client1` : `listen CAST1`.
7. Envoyer un nouveau message à diffuser : `mess localhost 7000`.
8. Écouter les messages diffusés avec le `client1` : `listen CAST1`.
9. Récupérer les 5 derniers messages diffusés avec le `client1` : `last localhost 7000`.
10. Enregistrer un nouveau `multicaster` :
`./build/multicaster CAST2 7001 225.1.2.4:9799 192.168.70.236:6900 config/multcast-mess1.conf`.
11. Redemander la liste des `multicaster` enregistrés : `list localhost 6900`.
12. Demander la météo de Paris au `CAST2` : `weather localhost 7001`.
13. Écouter les messages diffusés avec le `client1` : `listen CAST2`.
15. Envoyer un nouveau message à diffuser (avec le `manager`) : `mess localhost 6900`.
13. Écouter tous les `multicasters` avec le `client1` : `listen CAST1 CAST2`.
14. Déconnecter le `CAST1`.
13. Réécouter tous les `multicasters` avec le `client1` : `listen CAST1 CAST2`.
