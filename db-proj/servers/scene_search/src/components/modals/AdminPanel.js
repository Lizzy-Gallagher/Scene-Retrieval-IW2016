import React, { Component } from 'react';

import {Modal, Button, ButtonGroup} from 'react-bootstrap'

class AdminPanel extends Component {
  constructor(props) {
    super(props);

    this.close = this.close.bind(this);
    this.open = this.open.bind(this);
    this.toggleAutosuggest = this.toggleAutosuggest.bind(this);
    this.toggleVis = this.toggleVis.bind(this);

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

  toggleAutosuggest() {
    this.props.toggleAutosuggest();
  }

  toggleVis() {
    this.props.toggleVis();
  }

  render() {
    return (
      <div>
        <Button
          onClick={this.open}
          style={{position: 'absolute', top: '10px', right: '10px', width: '50px'}}
        >
            <i className="fa fa-cog fa-2x" aria-hidden="true"></i>
        </Button>

        <Modal show={this.state.showModal} onHide={this.close}>
          <Modal.Header closeButton>
            <Modal.Title>Admin Settings</Modal.Title>
          </Modal.Header>
          <Modal.Body>
            <h4>Enable Autosuggest?</h4>
            <ButtonGroup>
                <Button active={!this.props.doEnableAutosuggest} onClick={this.toggleAutosuggest}>No</Button>
                <Button active={this.props.doEnableAutosuggest} onClick={this.toggleAutosuggest}>Yes</Button>
            </ButtonGroup>
            <hr />
            <h4>Enable Enhanced Visualizations?</h4>
            <ButtonGroup>
                <Button active={!this.props.doEnableVis} onClick={this.toggleVis}>No</Button>
                <Button active={this.props.doEnableVis} onClick={this.toggleVis}>Yes</Button>
            </ButtonGroup>
            {/*<h4>Show Intermediate Representation?</h4>
            <ButtonGroup>
                <Button active={!this.props.doEnableVis} onClick={this.toggleVis}>No</Button>
                <Button active={this.props.doEnableVis} onClick={this.toggleVis}>Yes</Button>
            </ButtonGroup>*/}
            {/*<h4>Enable Advanced User Mode? (TAKES INT QUERIES</h4>
            <ButtonGroup>
                <Button active={!this.props.doEnableAdvUser} onClick={this.toggleVis}>No</Button>
                <Button active={this.props.doEnableAdvUser} onClick={this.toggleVis}>Yes</Button>
            </ButtonGroup>*/}
          </Modal.Body>
          <Modal.Footer>
            <Button onClick={this.close}>Close</Button>
          </Modal.Footer>
        </Modal>
      </div>
    )}
}

export default AdminPanel;
