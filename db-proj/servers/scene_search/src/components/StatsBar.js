import React, { Component } from 'react';

import {Row, Col, Table} from 'react-bootstrap';
import SearchOptions from './SearchOptions';

class StatsBar extends Component {
    constructor(props) {
        super(props);
    }

    render() {
        let element = this.props.numSceneResults > 0 ?
            <Row>
                <Col md={4}>
                    <Table striped bordered condensed>
                        <thead>
                            <tr>
                                <th>Scenes</th>
                                <th>Levels</th>
                                <th>Rooms</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td>{this.props.numSceneResults}</td>
                                <td>{this.props.numLevelResults}</td>
                                <td>{this.props.numRoomResults}</td>
                            </tr>
                        </tbody>
                    </Table>
                </Col>
                <Col md={1}>
                    <SearchOptions handleSelectChange={ this.props.handleSelectChange } />
                </Col>
            </Row>
         :
            <div></div>
        
        return (
            <div>
                { element }
            </div>
        );
  }
}

export default StatsBar;