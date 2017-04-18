import React, { Component } from 'react';
import {Modal, Button} from 'react-bootstrap'

import IconAttribution from '../IconAttribution'

class InfoPanel extends Component {
  constructor(props) {
    super(props);

    this.close = this.close.bind(this);
    this.open = this.open.bind(this);

    this.state = {
      showModal: false
    }
  }

  getInitialState() {
    return { showModal: false };
  }

  close() {
    this.setState({ showModal: false });
  }

  open() {
    this.setState({ showModal: true });
  }

  render() {
    return (
      <div>
        <Button
          onClick={this.open}
          style={{position: 'absolute', top: '10px', right: '70px', width: '50px'}}
        >
            <i className="fa fa-info-circle fa-2x" aria-hidden="true"></i>
        </Button>

        <Modal show={this.state.showModal} onHide={this.close}>
          <Modal.Header closeButton>
            <Modal.Title>Information</Modal.Title>
          </Modal.Header>
          <Modal.Body>
              <h4>About</h4>
              <p>SUNCG Search is the culminating project of Elizabeth Bradley's thesis research. It seeks to...</p>
              <hr/>
              <h4>Icon Attribution</h4>
              <IconAttribution />
          </Modal.Body>
         
          <Modal.Footer>
            <Button onClick={this.close}>Close</Button>
          </Modal.Footer>
        </Modal>
      </div>
    )}
}

export default InfoPanel;
