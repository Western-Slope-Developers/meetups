from flask import Flask, Response, render_template

from utils import udp_mcast_listen

app = Flask(__name__)


@app.route("/")
def index():
    return render_template('index.html', static_url_path='/static')

@app.route('/messages')
def messages():
    def _messages():
        for datagram in udp_mcast_listen():
            print(str(datagram[0])[2:-1])
            yield 'data: {}\n\n'.format(str(datagram[0])[2:-1])
    return Response(_messages(), mimetype="text/event-stream")


if __name__ == "__main__":
    app.run(threaded=True, debug=True)
