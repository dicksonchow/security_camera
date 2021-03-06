from pyramid.config import Configurator
from pyramid_beaker import session_factory_from_settings


def main(global_config, **settings):
    """ This function returns a Pyramid WSGI application.
    """
    session_factory = session_factory_from_settings(settings)
    config = Configurator(settings=settings)
    config.set_session_factory(session_factory)
    config.include('pyramid_chameleon')
    config.add_static_view('static', 'static', cache_max_age=3600)
    config.add_route('login', '/')
    config.add_route('logout', '/logout')
    config.add_route('reg', '/reg')
    config.add_route('auth', '/auth')
    config.add_route('home', '/home')
    config.add_route('admin', '/admin')
    config.add_route('upload', '/upload')
    config.add_route('statusUpdate', '/statusupdate')
    config.scan()
    return config.make_wsgi_app()
