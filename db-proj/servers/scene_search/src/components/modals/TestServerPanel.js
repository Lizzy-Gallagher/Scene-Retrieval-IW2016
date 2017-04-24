import React, { Component } from 'react';
import {Modal, Button} from 'react-bootstrap'

let thumbsUp = <i className="fa fa-thumbs-up fa-2x" style={{color: 'green'}} aria-hidden="true"></i>;
let thumbsDown = <i className="fa fa-thumbs-down fa-2x" style={{color: 'red'}} aria-hidden="true"></i>;

class TestServerPanel extends Component {
  constructor(props) {
    super(props);

    this.pingServers = this.pingServers.bind(this);
    this.close = this.close.bind(this);
    this.open = this.open.bind(this);

    this.state = {
      showModal: false,
      databaseResult: false,
      nlpResult: false,
      imgResult: false
    }
  }

  getInitialState() {
    return { showModal: false };
  }

  pingServers() {
    let self = this;
    if (!this.state.databaseResult) {
     fetch(this.props.databaseURL + 'ping')
      .then(function(response) {
        return response.json()
      }).then(function(json) {
        self.setState({databaseResult: true}) 
      })
    }

    if (!this.state.nlpResult) {
     fetch(this.props.nlpURL + 'ping')
      .then(function(response) {
        return response.json()
      }).then(function(json) {
        self.setState({nlpResult: true}) 
      })
    }

    if (!this.state.imgResult) {
     fetch(this.props.imgURL + 'ping')
      .then(function(response) {
        return response.json()
      }).then(function(json) {
        self.setState({imgResult: true}) 
      })
    }
  }

  close() {
    this.setState({
      showModal: false,
      databaseResult: false,
      nlpResult: false,
      imgResult: false
    });
  }

  open() {
    this.setState({ showModal: true });
    this.pingServers();
  }

  render() {
    let databaseIcon = this.state.databaseResult ? thumbsUp : thumbsDown;
    let nlpIcon = this.state.nlpResult ? thumbsUp : thumbsDown;
    let imgIcon = this.state.imgResult ? thumbsUp : thumbsDown;

    return (
      <div>
        <Button
          onClick={this.open}
          style={{position: 'absolute', top: '10px', right: '130px', width: '50px'}}
        >
            <i className="fa fa-server fa-2x" aria-hidden="true"></i>
        </Button>

        <Modal show={this.state.showModal} onHide={this.close}>
          <Modal.Header closeButton>
            <Modal.Title>Information</Modal.Title>
          </Modal.Header>
          <Modal.Body>
            <h4>Database Server: { databaseIcon }</h4>
            <h4>NLP Server { nlpIcon }</h4>
            <h4>Image Server { imgIcon }</h4>
          </Modal.Body>
          <Modal.Footer>
            <Button onClick={this.close}>Close</Button>
          </Modal.Footer>
        </Modal>
      </div>
    )}
}

export default TestServerPanel;
