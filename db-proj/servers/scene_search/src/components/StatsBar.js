import React, { Component } from 'react';

import {Grid, Row, Col, Panel} from 'react-bootstrap';

class StatsBar extends Component {
    constructor(props) {
        super(props);
    }

    render() {
        let element = this.props.numSceneResults > 0 ?
        <Panel bsStyle="success">
            <Grid>
                <Row className="show-grid">
                    <Col md={3}>{'Matches scenes: ' + this.props.numSceneResults}</Col>
                    <Col md={3}>{'Matching levels: ' + this.props.numLevelResults}</Col>
                    <Col md={3}>{'Matching rooms: ' + this.props.numRoomResults}</Col>
                </Row>
            </Grid>
         </Panel>:
            <div></div>
        
        return (
            <div>
                { element }
            </div>
        );
  }
}

export default StatsBar;