import os
import cv2
import uuid
import time
import shutil
import hashlib
import MySQLdb
import connect_to_database
from pyramid.view import view_config
from pyramid.response import Response
from pyramid.httpexceptions import HTTPForbidden, HTTPFound, HTTPNotFound


def security_checking(session):
    if 'username' not in session:
        raise HTTPForbidden()

    if session['username'] != 'Admin':
        raise HTTPForbidden


@view_config(route_name='login', renderer='templates/login.pt')
def forward_login_page(request):
    return {'page_title': 'Login Page'}


@view_config(route_name='home', renderer='templates/home.pt')
def forward_home_page(request):
    security_checking(request.session)
    return {'page_title': 'Home Page'}


@view_config(route_name='reg', renderer='templates/registration.pt')
def forward_user_reg_page(request):
    security_checking(request.session)
    return {'page_title': 'Users Registration Page'}


@view_config(route_name='admin', renderer='templates/admin.pt')
def forward_admin_page(request):
    security_checking(request.session)

    SELECT_USERS_QUERY = 'SELECT id, name, register FROM secamuser_tb;'
    results = connect_to_database.execute_select_query(SELECT_USERS_QUERY)

    return {
        'page_title': 'Administration Page',
        'user_list': results,
    }


@view_config(route_name='auth')
def authentication(request):
    username = 'Admin'
    password = 'c7ad44cbad762a5da0a452f9e854fdc1e0e7a52a38015f23f3eab1d80b931dd472634dfac71cd34ebc35d16ab7fb8a90c81f975113d6c7538dc69dd8de9077ec'

    if request.method != 'POST':
        raise HTTPForbidden()

    if request.params['username'] == username and hashlib.sha512(request.params['password']).hexdigest() == password:
        session = request.session
        session['username'] = username

        url = request.route_url('home')
        return HTTPFound(location=url)
    else:
        raise HTTPForbidden()


@view_config(route_name='logout')
def destroy_session(request):
    request.session.delete()
    url = request.route_url('login')
    return HTTPFound(location=url)


@view_config(route_name='upload')
def handle_upload_pictures(request):
    if request.method != 'POST':
        raise HTTPForbidden()
    security_checking(request.session)

    file_list = []
    for item in request.POST.iteritems():
        if item[0] == 'file':
            input_file = item[1].file
            file_name = '{}.jpg'.format(uuid.uuid4())
            file_list.append(file_name)
            file_path = os.path.join('temp_pictures', file_name)
            with open(file_path, 'wb') as output:
                shutil.copyfileobj(input_file, output)
        elif item[0] == 'user':
            print item[1]
        else:
            # very likely an attack
            raise HTTPForbidden()

    count = 0
    meta = '/usr/share/faces_meta/'
    facedb = '/usr/share/faces_db/'
    face_cascade = cv2.CascadeClassifier(meta + 'haarcascade_frontalface_alt.xml')

    if face_cascade.empty():
        raise HTTPNotFound()

    time.sleep(1)
    num_of_users = len([d for d in os.listdir('/usr/share/faces_db/') if os.path.isdir(os.path.join(facedb, d))])

    for f in file_list:
        temp_file = meta + 'temp_pictures/' + f
        if not os.path.isfile(temp_file):
            print 'Not a file'

        img = cv2.imread(temp_file)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        faces = face_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30),
                                              flags=cv2.cv.CV_HAAR_SCALE_IMAGE)

        print 'Found {} faces'.format(len(faces))

        for fa in faces:
            x, y, w, h = [v for v in fa]
            count += 1
            face_file_name = '/home/dickson/PycharmProjects/adminpage/faces/s' + str(count) + '.pgm'
            cv2.rectangle(img, (x, y), (x + w, y + h), (255, 255, 255))
            sub_face = cv2.resize(img[y:y + h, x:x + w], (92, 112), fx=1.0, fy=1.0, interpolation=cv2.INTER_CUBIC)
            cv2.imwrite(face_file_name, cv2.cvtColor(sub_face, cv2.COLOR_BGR2GRAY))

    return Response('Hello World')


@view_config(route_name='statusUpdate')
def user_status_update(request):
    if request.method != 'POST':
        raise HTTPForbidden()
    security_checking(request.session)

    SQL_UPDATE_QUERY = ''
    for uid in request.params.getall('status_up'):
        SQL_UPDATE_QUERY += 'UPDATE secamuser_tb SET register = NOT register WHERE id = {};\n'.format(
            MySQLdb.escape_string(uid))

    connect_to_database.execute_update_query(SQL_UPDATE_QUERY)
    url = request.route_url('home')
    return HTTPFound(url)
