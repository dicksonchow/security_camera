#!/bin/bash

PARMPICLOCATION=/usr/share/faces_db
PARMINSERTQUERY=""

for user in $(ls -1 $PARMPICLOCATION | sort -t 's' -nk 2);do
	if [ "$user" = "s41" ];then
		PARMINSERTQUERY="${PARMINSERTQUERY}INSERT INTO secamuser_tb(name, register) VALUES('Dickson Chow', true);\n"
	else
		PARMINSERTQUERY="${PARMINSERTQUERY}INSERT INTO secamuser_tb(name, register) VALUES('unknow_user', false);\n"
	fi
	PARMINSERTQUERY="${PARMINSERTQUERY}SELECT @last := LAST_INSERT_ID();\n"
	for pic in $(ls -1 $PARMPICLOCATION/$user | sort -t '.' -nk 1);do
		PARMINSERTQUERY="${PARMINSERTQUERY}INSERT INTO secamdata_tb(user_id, pic_path) VALUES(@last, '$PARMPICLOCATION/$user/$pic');\n"
	done
done

echo -e "The script has finished generating the SQL insert queries.\nEnter the password for execution."
mysql -u secamadm -p -e "$PARMINSERTQUERY" secamadm
