Για τη δημιουργία εκτελέσιμου τρέξτε:
gcc -o crossw arguments.c bitmap.c crossw.c dvo_fc_cbj_solver.c entities.c input_funcs.c

Το πρόγραμμα είναι δομημένο με τον εξής τροπο:
  -Έχει δημιουργηθεί η δομη struct slot στην οποία αποθηκεύονται πληροφορίες για τη λέξη προς συμπλήρωση.
  -Εκτός απο το dictionary που ειναι αποθηκευμενο σε εναν τρισδιαστατο πινακα υπαρχει και ο πινακας bitmap. Ο πινακας αυτος παιρνει ως τιμες συμβολοσειρες bit, που προσδιοριζουν αν η λεξη που αντιστοιχει στο αναλογο index του 
dictionary εχει τις ιδιοτητες που προσδιοριζονται απο τον bitmap.
  -Υπαρχει ενα look up table το οποιο για καθε Index του παιρνει ως τιμη τον αριθμο των set bits. Μας επιτρεπει να υπολογιζουμε γρηγορα το domain size, αφου το current domain καθε slot ειναι αποθηκευμενο σαν συμβολοσειρα.
  -Η λυση του σταυρολεξου χρησιμοποιει τις μεθοδους forward checking σε συδυασμο με conflict backjumping.
  -Το heuristic που χρησιμοποιειται ειναι το Most Restrained Value και η σειρα συμπληρωσης των slot ειναι δυναμικη, δηλαδη προσδιοριζεται κατα την επιλυση του σταυρολεξου. Ειναι σημαντικο να σημειωθει πως το mrv δε χρησιμοποιειται
για την επιλογη του πρωτου slot. Η επιλογη του πρωτου slot γινεται με βαση των αριθμο των intercepts που σχηματιζει με τα υπολοιπα slots.

Πηγες:
  -Ο αλγόριμος του forward-checking και confict backjumping ειναι βασισμενος στη δημοσιευση του P. Prosser, Hybrid Algorithms for the Constraint Satisfaction Problem.Computational Intelligence, Volume 9, Number 3. 1993
  -Η δημοσιευση του stanford "Crossword Puzzles and Constraint Satisfaction" εδωσε την ιδεα για τη δημιουργια του bitmap, καθως και για τη βελτιωση του forward-checking.
  -Το look up table βασιστηκε στον αλγοριθμο που παρουσιαζεται στη σελιδα https://bisqwit.iki.fi/source/misc/bitcounting/
