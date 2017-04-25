/**
 * Created by erb2 on 2/8/17.
 */

import React, { Component } from 'react';
import { FormGroup, ControlLabel, FormControl, Row } from 'react-bootstrap'

class SearchOptions extends Component {
  render() {
    return (
       <FormGroup controlId="formControlsSelect">
          <Row>
            <ControlLabel>View by:</ControlLabel>
          </Row>
          <Row>
            <FormControl componentClass="select"
                        onChange={ this.props.handleSelectChange }>
              <option value="scene">Scene</option>
              <option value="level">Level</option>
              <option value="room">Room</option>
            </FormControl>
          </Row>
      </FormGroup>
    );
  }
}

export default SearchOptions;
