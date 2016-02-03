import MySQLdb

hostname = '127.0.0.1'
username = 'secamadm'
password = 'p@ssw0rd'
dbname = 'secamadm'
port = 3306


def execute_select_query(sql_query):
    try:
        db = MySQLdb.connect(hostname, username, password, dbname, port)
        cursor = db.cursor(cursorclass=MySQLdb.cursors.DictCursor)
        cursor.execute(sql_query)
        results = cursor.fetchall()
        cursor.close()
        db.close()

        return results
    except MySQLdb.Error as e:
        print e.message


def execute_update_query(sql_query):
    try:
        db = MySQLdb.connect(hostname, username, password, dbname, port)
        cursor = db.cursor()
        cursor.execute(sql_query)
        cursor.close()
        db.commit()
        db.close()
    except Exception as e:
        db.rollback()
